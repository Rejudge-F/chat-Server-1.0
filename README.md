# chat-Server
使用方法：

$: ./server [PORT]
  
$: ./client [IP] [PORT] [ID] [PASSWD] [OP]

如果参数中的OP为1则进入注册模式，注册账号即为所填写的ID，密码为PASSWD， 昵称将会提示输入。

如果参数中的OP为0则进入登录模式，正常登录即可。
  
消息发送格式：[message]->[user_id](无中括号)

目的实现：端对端的一对一聊天，开放注册，登录，加好友等操作。
# 0.1.0版本
- 主要实现客户端与服务端的即时通讯，下一版本将实现客户端对客户端的即时通讯。
# 0.2.0版本
- 实现多人即时聊天，消息显示在服务器端，去掉0.1.0版本的客户端与服务器之间的即时通讯。
# 0.2.1版本
- 在0.2.0版本的基础上增加服务器广播功能。
# 0.2.2版本
做了如下改变：
  - 服务器从多进程改为select轮询方式进行广播和接收信息，提高服务器端运行效率，轮询时间：0.5s。
  - 增加R-BTree数据结构动态维护用户数量。
# 0.3.0版本
做了如下改变
  - 服务器端增加账号密码验证，本次只更新了一个用户admin，下一版本将采用数据库验证。
  - 更改客户端登录模式，从任意登录改为需要账号密码登录。
# 0.4.0版本
做了如下改变：
  - 服务器端代码重构，实现面向对象编程，更加容易理解，增加用户类保存在线用户的信息，并通过mysql管理用户信息。
  - 服务器信息需要在服务器源码的宏定义中预先定义。
# 0.5.0版本
  - 加入了Message类，在发送消息的时候进行处理，消息发送格式：`message->user_id`。
  - 在服务器广播没有取消的情况下增加了一对一聊天，须按照标准格式发送消息。
  - 修改makefile，更加直观看到链接的情况。
  - 下一版本将会成为最后一个版本，提高容错性，增加注册和加好友功能。
# 0.6.0版本
  - 修复已知错误。
  - 原功能并未改变。
  - 增加注册功能，修改客户端代码，增加参数。
