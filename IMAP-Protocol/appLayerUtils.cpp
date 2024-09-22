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
    std::cout << line << std::endl;
  
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
    std::cout << "C: " << command << std::endl;
  SSL_write(sslConnection, command.c_str(), command.length());
  std::string response = imapRecv(sslConnection, 100);
  int isOk = checkOK(response);
  if (!silent)
    std::cout << "S: " << response << std::endl;
  data.statusCode = isOk;
  data.message = response;

  return data;
}