project_name/

│

├── CMakeLists.txt

├── Makefile

├── README.md

├── build_project.sh

│

├── config/

│   ├── client_config.json

│   └── server_config.json

│

├── include/

│   ├── common.hpp

│   └── message_structs.hpp

│

├── src/

│   ├── server/

│   │   ├── main.cpp

│   │   ├── Server.cpp

│   │   ├── Server.hpp

│   │   ├── ClientHandler.cpp

│   │   └── ClientHandler.hpp

│   │

│   ├── client/

│   │   ├── main.cpp

│   │   ├── Client.cpp

│   │   ├── Client.hpp

│   │   ├── UI.cpp

│   │   └── UI.hpp

│   │

│   ├── protocol/

│   │   ├── Protocol.cpp

│   │   ├── Protocol.hpp

│   │   ├── Network.cpp

│   │   └── Network.hpp

│   │

│   ├── utils/

│   │   ├── Logger.cpp

│   │   ├── Logger.hpp

│   │   ├── Parser.cpp

│   │   └── Parser.hpp

│   │

│   └── db/

│       ├── Database.cpp

│       └── Database.hpp

│

├── logs/

│   ├── server.log

│   └── client.log

│

└── test/

├── test_message_format.cpp

└── test_network_connection.cpp