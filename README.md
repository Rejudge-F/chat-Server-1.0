# chat-Server
使用方法：

$: ./server \\<PORT>
  
$: ./client \\<IP> \\<PORT>
  
  
C++使用fork实现服务器与客户端之间聊天。
# 0.1.0版本
- 主要实现客户端与服务端的即时通讯，下一版本将实现客户端对客户端的即时通讯。
# 0.2.0版本
- 实现多人即时聊天，消息显示在服务器端，去掉0.1.0版本的客户端与服务器之间的即时通讯
# 0.2.1版本
- 在0.2.0版本的基础上增加服务器广播功能
# 0.2.2版本
做了如下改变：
  - 服务器从多进程改为select轮询方式进行广播和接收信息，提高服务器端运行效率，轮询时间：0.5s
  - 增加R-BTree数据结构动态维护用户数量
