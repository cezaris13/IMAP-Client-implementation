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
    std::print("{}\n",line);
  
  file.close();
}

std::string base64Decode(std::string const &encodedString) {
  const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "abcdefghijklmnopqrstuvwxyz"
                             "0123456789+/";
  int encodedStringLength = encodedString.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (encodedStringLength-- && (encodedString[in_] != '=') && isBase64(encodedString[in_])) {
    char_array_4[i++] = encodedString[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64Chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64Chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++)
      ret += char_array_3[j];
  }

  return ret;
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

std::unordered_map<std::string, std::string> loadEnv(const std::string& filePath) {
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
            value.erase(0, value.find_first_not_of(" \n\r\t")); // remove leading whitespace
            value.erase(value.find_last_not_of(" \n\r\t") + 1); // remove trailing whitespace

            // Store the key-value pair in the map
            envVariables[key] = value;
        }
    }

    envFile.close();
    return envVariables;
}