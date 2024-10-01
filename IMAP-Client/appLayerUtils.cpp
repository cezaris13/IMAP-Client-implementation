#include <print>
#include <unordered_map>

struct Data {
  int statusCode; // 1 for success 0 for failure
  std::string message;
};

bool isBase64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

void showFileContents(std::string fileName) {
  if (fileName.substr(fileName.length() - 4, 4) != ".txt")
    return;
  std::ifstream file;
  file.open(fileName);
  std::string line;
  while (getline(file, line))
    std::print("{}\n", line);

  file.close();
}

void decodeBase64Chunk(const unsigned char charArray4[4],
                       unsigned char charArray3[3]) {
  // 1st byte: take all 6 bits from the first character, and 2 bits from the
  // second character
  charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);

  // 2nd byte: take remaining 4 bits from the second character and 4 bits from
  // the third character
  charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);

  // 3rd byte: take the remaining 2 bits from the third character and all 6 bits
  // from the fourth character
  charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];
}

std::string base64Decode(std::string const &encodedString) {
  const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "0123456789+/";
  int encodedLength = encodedString.size();
  int base64Index = 0;
  unsigned char charArray4[4], charArray3[3];
  std::string decodedString;
  int chunkSize = 0;

  while (encodedLength-- && (encodedString[base64Index] != '=') &&
         isBase64(encodedString[base64Index])) {
    charArray4[chunkSize++] = encodedString[base64Index++];

    // When we have 4 valid base64 characters, convert them to 3 decoded bytes
    if (chunkSize == 4) {
      for (int i = 0; i < 4; i++) {
        charArray4[i] = base64Chars.find(charArray4[i]);
      }
      decodeBase64Chunk(charArray4, charArray3);

      // Append the decoded characters to the result
      for (int i = 0; i < 3; i++) {
        decodedString += charArray3[i];
      }
      chunkSize = 0; // Reset chunk size
    }
  }

  // Process any remaining characters (padding or incomplete chunks)
  if (chunkSize > 0) {
    // Fill remaining positions in the 4-character array with zero
    for (int i = chunkSize; i < 4; i++) {
      charArray4[i] = 0;
    }

    // Convert base64 characters to indexes
    for (int i = 0; i < 4; i++) {
      charArray4[i] = base64Chars.find(charArray4[i]);
    }

    // Decode the remaining characters
    decodeBase64Chunk(charArray4, charArray3);

    // Append the remaining decoded bytes
    for (int i = 0; i < chunkSize - 1; i++) {
      decodedString += charArray3[i];
    }
  }

  return decodedString;
}

Data sendAndReceiveImapMessage(std::string command, SSL *sslConnection,
                               int silent) {
  Data data;
  if (!silent)
    std::print("C: {}\n", command);
  SSL_write(sslConnection, command.c_str(), command.length());
  std::string response = imapRecv(sslConnection, 100);
  int isOk = checkOK(response);
  if (!silent)
    std::print("S: {}\n", response);
  data.statusCode = isOk;
  data.message = response;
  if (data.statusCode == 0)
    std::print("{}", data.message);
  return data;
}

std::unordered_map<std::string, std::string>
loadEnv(const std::string &filePath) {
  std::unordered_map<std::string, std::string> envVariables;
  std::ifstream envFile(filePath);

  if (!envFile.is_open()) {
    std::cerr << "Error: Could not open the .env file." << std::endl;
    return envVariables;
  }

  std::string line;
  while (std::getline(envFile, line)) {
    // Ignore comments and empty lines
    if (line.empty() || line[0] == '#') {
      continue;
    }

    // Split the line at the '=' character
    size_t delimiterPos = line.find('=');
    if (delimiterPos != std::string::npos) {
      std::string key = line.substr(0, delimiterPos);
      std::string value = line.substr(delimiterPos + 1);

      // Remove any surrounding whitespace
      key.erase(key.find_last_not_of(" \n\r\t") + 1);
      value.erase(
          0, value.find_first_not_of(" \n\r\t")); // remove leading whitespace
      value.erase(value.find_last_not_of(" \n\r\t") +
                  1); // remove trailing whitespace

      // Store the key-value pair in the map
      envVariables[key] = value;
    }
  }

  envFile.close();
  return envVariables;
}