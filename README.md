```shell
-- 由于 **TinyRPC** 读取配置使用了 **xml** 文件，因此需要安装 **tinyxml** 库来解析配置文件。
下载地址：https://sourceforge.net/projects/tinyxml/
要生成 libtinyxml.a 静态库，需要简单修改 makefile 
如下:
# 84 行修改为如下
OUTPUT := libtinyxml.a 
# 194, 105 行修改如下
${OUTPUT}: ${OBJS}
	${AR} $@ ${LDFLAGS} ${OBJS} ${LIBS} ${EXTRA_LIBS}
	
$ cd tinyxml
$ make -j4
# copy 库文件到系统库文件搜索路径下
$ cp libtinyxml.a /usr/lib/
# copy 头文件到系统头文件搜索路径下 
$ mkdir /usr/include/tinyxml
$ cp *.h /usr/include/tinyxml
	
-- protobuf安装
protobuf 推荐使用 3.19.4 及其以上：
-- 安装过程：
$ wget  https://github.com/protocolbuffers/protobuf/releases/download/v3.19.4/protobuf-cpp-3.19.4.tar.gz
$ tar -xzvf protobuf-cpp-3.19.4.tar.gz
我们需要指定 安装路径在 /usr 目录下:
$ cd protobuf-cpp-3.19.4
$ ./configure -prefix=/usr/local
$ make -j4
$ sudo make install
安装完成后，你可以找到头文件将位于 /usr/include/google 下，库文件将位于 /usr/lib 下。
```

```
1. 环境安装与项目开发
1.1 环境搭建和依赖库安装
1.2 日志模块开发
1.3 配置模块开发

2. EventLoop 模块封装
2.1 EventLoop 核心类构建
2.2 FdEvent 封装以及测试
2.3 定时器 Timer
2.4 主从 Reactor 
2.5 EventLoop 模块整体测试

3. Tcp 模块封装
3.1 TcpBuffer
3.2 TcpConnection 
3.3 TcpServer (一)
3.4 TcpServer (二)
3.4 TcpClient
3.5 Tcp 模块测试 (一)
3.5 Tcp 模块测试 (二)



4. RPC 协议封装
4.1 TinyPB 协议编码
4.2 TinyPB 协议解码
4.3 编解码模块测试

5. RPC 通信模块封装
5.1 RpcController 以及 RcpClosure 等基础类
5.2 RpcDispatcher 分发器
5.3 RpcChannel
5.4 RpcAsyncChannel
5.5 Rpc 模块集成测试

6. RPC 脚手架封装
6.1 代码生成器开发
6.2 项目的构建与测试
```

![img](https://img-blog.csdnimg.cn/036ddabcb41a4937a69f1b009ad270cf.png)

# RPC安装

```sql
-- 安装：
	cd testcases
	protoc --cpp_out=./ order.proto
	cd ..
	make -j4
	sudo make install 
-- 检查是否安装 
	cd /usr/lib/
	ls //查看是否有这个librocket.a
-- 执行rocket 脚手架部署
1 创建protobuf文件，例如order_server.proto //这个python 脚本就是为了创建 socket 项目
2 使用python脚本，创建项目文件，
	/home/kevin/rpc/rocket/rocket-main/generator/rocket_generator.py -i protobuf文件 -o 输出路径
3 生成rpc项目

4 生成之后 在makefile 文件目录下make
	cd /home/kevin/rpc/rocket/order_server/order_server make;
5 make成功之后
	在bin/生成 可执行文件order_server
	testtool/下会生成测试文件
6 运行 进入到bin
	sh run.sh ../conf/rocket.xml
	以守护进程的方式在后台
```

```sql
# 在Socket编程中，read、write、recv和send是用于在网络套接字上进行读写操作的函数。它们的主要区别如下：

read函数：

1 read函数是在文件描述符上进行读取操作的通用函数，包括网络套接字。
2 read函数的原型为ssize_t read(int fd, void *buf, size_t count)。
3 read函数会从文件描述符fd中读取最多count个字节的数据，并将其存储到buf指向的缓冲区中。
4 read函数返回实际读取的字节数，或者在出现错误时返回-1。

write函数：

1 write函数是在文件描述符上进行写入操作的通用函数，包括网络套接字。
2 write函数的原型为ssize_t write(int fd, const void *buf, size_t count)。
3 write函数会将buf指向的缓冲区中的count个字节数据写入到文件描述符fd中。
4 write函数返回实际写入的字节数，或者在出现错误时返回-1。

recv函数：

1 recv函数是用于从网络套接字接收数据的函数。
2 recv函数的原型为ssize_t recv(int sockfd, void *buf, size_t len, int flags)。
3 recv函数会从套接字sockfd中接收最多len个字节的数据，并将其存储到buf指向的缓冲区中。
4 recv函数还可以通过flags参数指定一些额外的选项，如设置接收标志等。
5 recv函数返回实际接收的字节数，或者在出现错误时返回-1。

send函数：

1 send函数是用于向网络套接字发送数据的函数。
2 send函数的原型为ssize_t send(int sockfd, const void *buf, size_t len, int flags)。
3 send函数会将buf指向的缓冲区中的len个字节数据发送到套接字sockfd中。
4 send函数还可以通过flags参数指定一些额外的选项，如设置发送标志等。
5 send函数返回实际发送的字节数，或者在出现错误时返回-1。

总结：
read和write是通用的文件描述符读写函数，可以用于网络套接字。而recv和send是专门用于网络套接字的接收和发送数据的函数。它们的主要区别在于接口和使用方式上的细微差别。
```

```
@@ 		是互斥锁
!! 		是信号量
&&		条件变量
```



# 模块

## 全局信息模块

### 配置模块

```c++
-- 功能：
    完成xml文件的解析；
-- 头文件 config.h
-- 成员变量：
公有：
    日志等级
    日志文件名
    日志文件路径
    最大日志文件
    日志同步时间间隔
    端口号
    io线程号
    tinyxmldcument
-- 成员函数：
普通：
    构造函数（有参，无参）//解析xml
    析构
静态：
    获取静态全局配置参数；
    设置全局配置参数;
    
    
    
struct RpcStub
    {
        std::string name; 
        NetAddr::s_ptr addr;
        int timeout{2000};
    };
    class Config
    {
    public:
        Config(const char *xmlfile);
        Config();//解析xml
        ~Config();
    public:
        static Config *GetGlobalConfig();//获取静态全局的配置参数
        static void SetGlobalConfig(const char *xmlfile);//设置全局配置参数，就是有就覆盖，没有就去调用构造函数
    public:
        std::string m_log_level; //日志等级
        std::string m_log_file_name;
        std::string m_log_file_path;
        int m_log_max_file_size{0};
        int m_log_sync_inteval{0}; // 日志同步间隔，ms
        int m_port{0};
        int m_io_threads{0};
        TiXmlDocument *m_xml_document{NULL};
        std::map<std::string, RpcStub> m_rpc_stubs;
    };
```

### 错误码

```c++
-- 功能 
    RPC的错误信息码

const int ERROR_PEER_CLOSED = SYS_ERROR_PREFIX(0000);    // 连接时对端关闭
const int ERROR_FAILED_CONNECT = SYS_ERROR_PREFIX(0001);  // 连接失败
const int ERROR_FAILED_GET_REPLY = SYS_ERROR_PREFIX(0002);  // 获取回包失败
const int ERROR_FAILED_DESERIALIZE = SYS_ERROR_PREFIX(0003);    // 反序列化失败
const int ERROR_FAILED_SERIALIZE = SYS_ERROR_PREFIX(0004);      // 序列化 failed

const int ERROR_FAILED_ENCODE = SYS_ERROR_PREFIX(0005);      // encode failed
const int ERROR_FAILED_DECODE = SYS_ERROR_PREFIX(0006);      // decode failed

const int ERROR_RPC_CALL_TIMEOUT = SYS_ERROR_PREFIX(0007);    // rpc 调用超时

const int ERROR_SERVICE_NOT_FOUND = SYS_ERROR_PREFIX(0008);    // service 不存在
const int ERROR_METHOD_NOT_FOUND = SYS_ERROR_PREFIX(0009);    // method 不存在 method 
const int ERROR_PARSE_SERVICE_NAME = SYS_ERROR_PREFIX(0010);    // service name 解析失败
const int ERROR_RPC_CHANNEL_INIT = SYS_ERROR_PREFIX(0011);    // rpc channel 初始化失败
const int ERROR_RPC_PEER_ADDR = SYS_ERROR_PREFIX(0012);    // rpc 调用时候对端地址异常

```

### 异常处理

```c++
-- 头文件exception.h
成员变量：
保护：
	错误码
    错误信息
函数：
    构造
    获取错误码
    获取错误信息
    异常捕获//捕获异常后执行该函数
```

### 工具

```
//获取进程id
pid_t getPid();
//获取线程id
pid_t getThreadId();
//获取现在时间
int64_t getNowMs();
///*ntohl()将一个无符号长整形数从网络字节顺序转换为主机字节顺序，*/
int32_t getInt32FromNetByte(const char *buf);
{
	int32_t re;
	memcpy(&re, buf, sizeof(re));
	return ntohl(re);
}
```

### 消息id模块

```c++
-- 头文件：Msg_ID_Util.h
功能： 生成消息id
函数：
	生成消息id
# 线程局部变量：
    static thread_local std::string t_msg_id_no;  		//线程局部变量//当前id
    static thread_local std::string t_max_msg_id_no;  	//最大id
# 全局静态变量
    static int g_msg_id_length = 20; //msg——id默认
    static int g_random_fd = -1;     //随机fd
	
```


### locker

```sql
-- 封装互斥锁
-- mutex.h
-- 对锁进行封装
```

## 日志模块

```
全局变量：
	enum LogLevel
    {
        Unknown = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    };
全局函数：
	LogLevelToString(LogLevel level) 				//日志转换
	StringToLogLevel(const std::string &log_level)	 //日志转换
	CoredumpHandler(int signal_no) 					//内核中断句柄
日志打印模块：都是宏定义
字符串转换函数：模板
	template <typename... Args>
	std::string formatString(const char *str, Args &&...args) //格式化字符串用于日志打印
```

```sql
-- 日志事件 LogEvent 
-- 功能就是日志类
-- 类名：LogEvent 

-- 变量：
	#智能指针
 	1 文件名
	2 行号
 	3 进程号
	4 线程号
	5 日志级别
-- 方法：
	1 构造（日志级别）
	2 获取日志文件名字
	3 获取日志等级
	4 将日志转换为字符串{日志：进程id，线程id，时间，事件/错误信息}
```

```sql
-- 日志打印器
-- 功能：打印日志
-- 类名：Logger

-- 静态：
	1 获取全局日志 { 获取g_logger }
	2 初始化全局日志 { 根据日志级别 new logger 调用成员：初始化方法 :3 }
-- 变量：
	#智能指针
	1 日志等级
	2 日志缓存buf
	3 app缓存buf
	4 @@ 互斥锁
	5 @@ app互斥锁
	6 日志输出文件名
	7 日志输出路径
	8 日志单个文件最大大小
	9 异步日志智能指针/app异步日志智能指针
	10 定时器
	11 日志类型
-- 方法：
	1 构造（等级，类型）
	2 push日志 {把日志push到队列里}
	3 初始化 {根据日志类型判断是否需要 添加定时器任务}
	4 刷新{往文件里flush}
	5 同步循环 {通过使用智能指针 9  同步变量 2 到异步日志buf里}
	6 获取日志等级 
	7 获取智能指针
```

```sql
-- 异步日志
-- 功能进行异步日志打印
-- 类名：AsyncLogger

-- 变量：
	1 日志队列
	2 日志输出文件名
	3 日志输出文件路径
	4 当前打印日志的日期
	5 日志单个文件最大大小
	6 && 条件变量
	7 日志文件句柄
	8 再打开标志
	9 停止标识
	10 !! 信号量
	11 @@ 互斥锁
-- 方法：
	1 构造函数（文件名，路径，最大值）
	2 停止使用异步日志
	3 刷新{刷新到日志文件中}
	4 push{往队列里push vector，vector来之logger日志}

```

## 主从模型模块

### fd事件

```c++
背景知识：
这个类就相当于封装了他：
struct epoll_event {
        uint32_t     events;      //Epoll events  这个就是那个enum
        epoll_data_t data;        // User data variable 这个就相当于这个联合体的fd类型
};
typedef union epoll_data {
        void        *ptr;
        int          fd;        // 通常情况下使用这个成员, 和epoll_ctl的第三个参数相同即可
        uint32_t     u32;
        uint64_t     u64;
} epoll_data_t;
```



```sql
-- 功能 就是封装struct epoll_event 
-- 头文件： fd_event.h
-- 类名： FdEvent

-- 全局：
	枚举：触发事件
	enum TriggerEvent
    {
		IN_EVENT = EPOLLIN,   	 #读事件
         OUT_EVENT = EPOLLOUT,	  #写事件
         ERROR_EVENT = EPOLLERR,  #epoll错误
    };
-- 变量：
保护：
	1 文件描述符
	2 监听用的epoll_event
	3 读事件的callback
	4 写事件的callback
	5 当epoll返回epollerr的时候的callback
-- 方法:
	1 构造
	2 析构
	3 设置fd非阻塞{fcntl}
	4 监听listen(event类型就是上面的触发事件,callback,错误时的callback){
		就是根据传入的event 去判断给那个回调函数指针赋值；
	}
	5 取消监听(event类型就是上面的触发事件){
		取消读写事件
	}
	6 获取文件描述符 变量 1 
	7 获取监听用epoll_event 变量 2 
	8 处理函数（event_type）
	{
		就是根据传入的event 去判断调用那个回调函数；
	}
```

wakeup_fd

```sql
-- wakeup fd 作用？

1 先搞清楚 Reactor 的原理，再来看这个问题。
在某些场景下，例如主从 Reactor 时。mainReactor 复制 listenfd 的监听，当客户端新连接过来时，accept获取 clientfd，然后把 clientfd 随机分配给一个 subReactor，由这个 subReactor 负责clientfd的后续读写操作。

2 而对于 subReactor 来说，它一直在不断执行 loop 循环，没有 IO 事件发生时都会沉睡在 epoll_wait 上。主要问题#假设 mainReactor 向某个 subReactor 添加 fd 的时候，subReactor正在 epoll_wait 沉睡，就会造成处理这个clientfd 延迟。

3 为了解决这个延迟，mainReactor 在添加clientfd给 subReactor 时，需要某种机制，把 subReactor 从 epoll_wait 唤醒，这个唤醒动作就叫 wakeup

4 如何实现唤醒，就是用 IO 事件触发，因此每个 Reactor 需要监听一个特殊的 wakeupfd 的可读事件，当需要唤醒时，只需要往这个 wakeupfd 里面写入数据，wakeupfd 马上变得可读，epoll_wait 就马上监听到可读事件，立马返回了。返回之后，就可以处理当前待执行任务。

#######
所以啊 ，wakefd实质上也是一个fd对吧；所以啊，我们就让他继承自fd_event
```

```sql
-- 功能 
假设 mainReactor 向某个 subReactor 添加 fd 的时候，subReactor正在 epoll_wait 沉睡，就会造成处理这个clientfd 延迟。为了解决这个延迟，mainReactor 在添加clientfd给 subReactor 时，需要某种机制，把 subReactor 从 epoll_wait 唤醒，这个唤醒动作就叫 wakeup
-- 头文件： wakeup_fd_event.h
-- 类名： WakeUpFdEvent ： public FdEvent{}
-- 变量：无
-- 方法：
	1 构造(fd):FdEvent(fd);
	2 析构
	3 唤醒操作()
	{
		往缓冲区里写8个字符
	}

```

### fd事件组

```sql
-- 功能 就是封装fd_event 形成一个组
-- 头文件： fd_event_group.h
-- 类名： FdEventGroup
-- 变量：
-私有：
	1 group的大小也就是fd_event的个数
	2 fd数组vector<fd_event>
	3 互斥锁
	
-- 方法：
-静态：
	1 获取fd_event_group 没有就new一个
-公有：
	1 狗造函数（size）{初始化大小，然后初始化size个fd_event};
	2 析构释放资源
	3 根据fd获取fd_event
	FdEvent *FdEventGroup::getFdEvent(int fd)
    {
        ScopeMutex<Mutex> lock(m_mutex);
        if ((size_t)fd < m_fd_group.size())
        {
            return m_fd_group[fd];
        }

        int new_size = int(fd * 1.5);
        for (int i = m_fd_group.size(); i < new_size; ++i)
        {
            m_fd_group.push_back(new FdEvent(i));
        }
        return m_fd_group[fd];
    }

```

### 定时器事件

```sql
-- 功能 : 定时器事件，里面有定时器任务的各种信息，每个event 就是一个定时器任务
-- 头文件： timer_event.h
-- 类名： TimerEvent
## 智能指针
  typedef std::shared_ptr<TimerEvent> s_ptr;
-- 变量：
-私有：
	1 执行时间
	2 时间间隔
	3 是否周期任务标识 bool
	4 是否取消标识 bool
	5 回调函数 fuction<void()> m_task;
-- 方法：
	1 构造函数（）{时间间隔，是否重复，回调函数}
	2 获取到达时间
	3 设置取消标记
	4 是否取消
	5 是否是周期任务
	6 获取回调函数
	7 重新设置执行时间{ 就是 现在的时间（调用util getNowMs） + 时间间隔（变量 2 ） }
```

### 定时器

```sql
-- 功能 : 就是封装了外部去使用定时任务的接口
-- 头文件： timer.h
-- 类名： Timer ：public FdEvent
-- 变量：
- 私有：
	1 multimap< int64_t ,TimerEvent::s_ptr> 待执行任务
	2 @@ 互斥锁
-- 方法：
- 公有：
	1 构造函数{
		创建一个定时器m_fd ：timerfd_create();
		// 把 fd 可读事件放到了 eventloop 上监听
        listen(FdEvent::IN_EVENT, std::bind(&Timer::onTimer, this));
	}
	2 析构函数
	3 添加定时任务(TimerEvent::s_ptr event){
		- 声明： 是否reset timer 标识
		- @ 加锁
		- 判断等待队列是否为空
		- 如果空 reset 
		- 否则 判断 是否新任务比任何一个任务都早执行，
		- 如果是 加到map里，然后 reset；
		- 然后根据reset标识看是否reset
		- 调用函数 6 
	}
	4 删除定时任务(TimerEvent::s_ptr event){
		- 设置event 取消标识
		- 加锁
		- 通过 lower_bound 和 upper_bound 找到event
		- 然后erase；
	}
	5 onTimer # 当发送了 IO 事件后，eventloop 会执行这个回调函数{
		- 处理缓冲区数据，防止下一次继续触发可读事件
		char buf[8];                                             	#
		while (1)												#
        {														#
            if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN)		 #
            {													#				
            	break;											#
            }													#
        }														#
		- 执行定时任务
		- 把所有符合条件的任务取出来放在vector里;
		- 然后再把周期任务添加到里面去
		{
			每个周期任务还要重新设置时间哦（这个是每个timer event 自己的reset）；
		}
		- 调用函数 6 重设执行时间 
		- 执行任务
	}
- 私有：
	6 重新设置执行时间{
		-加锁
		-复制 任务队列到tmp
		-解锁
		-判断 是否tmp 是空
			-是 return；
		- 获取现在时间now
		- 取出第一个任务 计算间隔
		- 判断 是否第一个任务的执行时间 大于 now
			- 是 时间间隔就等于 执行时间 - now；
			- 否 时间间隔就等于 100； 
		-最后，调用timerfd_settime函数来设置定时器。设置定时器的启动时间和间隔。
	}
```



### 事件循环

```c++
-- 功能 : 进行io事件监听
-- 头文件： eventloop.h
-- 类名： EventLoop
-- 静态:
	static EventLoop *GetCurrentEventLoop();#获取当前的EventLoop
-- 静态全局变量
    static thread_local EventLoop *t_current_eventloop = NULL;#当前线程的eventloop
    static int g_epoll_max_timeout = 10000; #最大超时事件
    static int g_epoll_max_events = 10;#最大epoll事件
-- 变量：
	1 pid_t m_thread_id{0};//线程id
	2 int m_epoll_fd{0};//epoll_fd;
	3 int m_wakeup_fd{0};// wakeup 用的fd
	4 WakeUpFdEvent *m_wakeup_fd_event{NULL};//唤醒的事件
	5 bool m_stop_flag{false};//停止标识
	6 std::set<int> m_listen_fds;#//用于监听的fd；
	7 std::queue<std::function<void()>> m_pending_tasks;//待执行任务
	8 Mutex m_mutex;//#互斥锁
	9 Timer *m_timer{NULL};//#定时器
	10 bool m_is_looping{false};//#循环
public:
	EventLoop();//#构造
	{	
		-当前线程下的event_loop 是否为空
		- 是空的
		- 获取线程id；
		- 创建epoll实体m_epoll_fd；
		- 断言epoll实体m_epoll_fd；
		- 初始化 wakeupfdevent；
		-初始化定时器
		- 线程局部变量t_current_eventloop = this；
	}
	~EventLoop();//#析构
	{
		- 关闭epoll实体；
		- delete wakeupfd
		- delete timer;
	}
	void loop();//#loop循环
	{
		- 设置 变量 10 为true;
		- 循环while(m_stop_flag);
		{
		第一阶段：执行上一次epollwait 取出的任务;
			- 加锁;
			- 取出待执行任务;
			- 解锁;
			- 如果 任务队列非空;
			{
				执行 任务队列的回调函数;
			}
		第二阶段：再次进行epollwait;
			- epoll_event result_events[g_epoll_max_events];
			- rt = epollwait(m_epoll_fd, result_events, g_epoll_max_events, g_epoll_max_timeout);
			# epoll_wait返回的是由有多少个;
			- 断言 epollwait（）;
			- 断言成功;
			{
				for (int i = 0; i < rt; ++i)
				{
					- epoll_event trigger_event = result_events[i];
					#取出这个event的fd ，用来初始化fdEvent;
                 	  - FdEvent *fd_event = static_cast<FdEvent *>(trigger_event.data.ptr);
                 	  - 根据trigger_event的事件类型判断要执行什么操作;
                 	  - 然后调用addtask(fd_event->handler(FdEvent::IN_EVENT或者FdEvent::OUT_EVENT);
                      - 如果是trigger_event.events & EPOLLERR;
                      - {
                             - 删除出错的fd;
                             - 调用addtask(添加错误时的任务）;
                        };
				};
			};
		};
	};
                                         
	void wakeup();//#唤醒{ 调用 wake_up_fd_event 的wakeup函数}；                                   
	void stop();//#停止loop{ 	m_stop_flag = true; 	 wakeup(); 		}
	void addEpollEvent(FdEvent *event);//#添加epoll事件
    {
        - 判断event是否在 loop线程中
        {
            如果在，就将他event 添加到epoll
            {
                - 在m_listen_fds中找一下看看这个事件有没有添加
                - 如果有，操作就是EPOLL_CTL_MOD，否则就是EPOLL_CTL_ADD
                - 然后通过event 找到他的tmp = event->epollevent ，
                - 然后使用epollctl（m_epoll_fd, op, event->getFd(), &tmp）;
                - m_listen_fds 添加event 的fd；
            }
            如果不在
            {
                -就把添加到epoll的回调函数添加到任务队列m_pending_tasks；
                    -这个回调函数{就是上面的函数的步骤；
                }
            }
        }
        
    }                                                              
	void deleteEpollEvent(FdEvent *event)//#删除epoll事件
    {
        - 判断event是否在 loop线程中
        {
            如果在，就将他event从epoll中删除
            {
                - 在m_listen_fds中找一下看看这个事件有没有添加
                - 如果没有，就返回，否则操作是EPOLL_CTL_DEL
                - 然后通过event 找到他的tmp = event->epollevent ，
                - 然后使用epollctl（m_epoll_fd, op, event->getFd(), &tmp）;
                - m_listen_fds erase 删除event 的fd；
            }
            如果不在
            {
                -就把添加到epoll的回调函数添加到任务队列m_pending_tasks；
                    -这个回调函数{就是上面的函数的步骤；
                }
            }
        }
        
	}
	bool isInLoopThread()#是否在loop线程 { return getThreadId() == m_thread_id; }
	void addTask(std::function<void()> cb, bool is_wake_up = false)//#添加任务
    {
        ScopeMutex<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();
        if (is_wake_up)
        {
            wakeup();
        }
    }
	void addTimerEvent(TimerEvent::s_ptr event);//#添加定时器任务
    {
        m_timer->addTimerEvent(event);
    }
	bool isLooping();//#是否在loop
    {
        return m_is_looping;
  	}
private:
	void dealWakeup();//#
	void initWakeUpFdEevent();//#初始化wakeup事件
    {
        //非阻塞的fd
        m_wakeup_fd = eventfd(0, EFD_NONBLOCK);
        if (m_wakeup_fd < 0){ exit(0);}
        m_wakeup_fd_event = new WakeUpFdEvent(m_wakeup_fd);
        m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [this]()//可读事件添加为有可读事件的回调函数
        {
    		char buf[8];
    		while(read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN) {}
		});
        addEpollEvent(m_wakeup_fd_event);
    }                                                                         
	void initTimer();//#初始化定时器
    {
        m_timer = new Timer();
        addEpollEvent(m_timer);//
    }
```

IO线程模块

#### IO线程

```sql
-- 功能 : 就是封装了外部去使用定时任务的接口
-- 头文件： io_thread.h
-- 类名： IOThread
-- 静态
	static void *Main(void *arg)	#静态函数//新线程入口，arg传入是ioThread的this指针
	{
		-创建一个新的eventloop；
		-创建完上面的部分才能让主线程去走
		-唤醒等待的线程（主线程）  sem_post(&thread->m_init_semaphore);
		-让IO 线程等待，直到我们主动的启动 sem_wait(&thread->m_start_semaphore);
		- 主动唤醒后执行thread->m_event_loop->loop();
	}
-- 变量：
- 私有：
	1 线程号
	2 线程句柄
	3 事件循环//当前 io 线程的 loop 对象
	4 !! sem_t m_init_semaphore;	#构造函数创建线程时让他等待子线程创建完eventloop
	5 !! sem_t m_start_semaphore; 	#控制启动的信号量
-- 方法
- 公有：
	1 构造{
		- 初始化信号量 4 ， 5；
		- 创建线程（&m_thread, NULL, &IOThread::Main, this)#静态函数作为线程入口
		- # wait, 直到新线程执行完静态 Main 函数的前置
	}
	2 析构{
		- 销毁信号量
		- 等待子线程完成
		- delete掉event_loop
	}
	3 获取当前线程的eventloop
	4 start #唤醒main函数的loop函数主动唤醒{
		sem_post(&m_start_semaphore);
	}
	5 join	{
		pthread_join(m_thread, NULL);
	}
```

#### IO线程组

```sql
-- 功能 : 就是封装了多个IO线程
-- 头文件： io_thread_group.h
-- 类名： IOThreadGroup
-- 变量
- 私有：
	1 线程组大小
	2 io线程数组
	3 线程索引
-- 方法：
- 公有：
	1 构造函数（size）{
		-初始化 线程组大小
		m_io_thread_groups.resize(size);
		-while(size--)
		{
			m_io_thread_groups[size] = new IOThread();
		}
	}
	2 析构
	3 start{
		for(auto i:m_io_thread_groups)
		{
			i.start();
		}
	}
	4 join{
		调用自己的join
	}
	5 获取一个IO线程
	{
		获取index表示的io线程
	}
```

## 协议抽象模块

### AbstractCoder

```sql
-- 功能 : 抽象编解码类，提供抽象的编码解码接口
-- 头文件： abstract_coder.h
-- 类名： AbstractCoder
-- 方法
        # 将 message 对象转化为字节流，写入到 buffer
        virtual void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer) = 0;

        # 将 buffer 里面的字节流转换为 message 对象
        virtual void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer) = 0;
		# 析构
        virtual ~AbstractCoder() {}
```

### AbstractProtocol

```sql
-- 功能 : 抽象协议结构体，提供抽象的协议结构体
-- 头文件： abstract_protocol.h
-- 类名： AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol>
-- 方法
  public:
    typedef std::shared_ptr<AbstractProtocol> s_ptr; #智能指针
    virtual ~AbstractProtocol() {}
  public:
    std::string m_msg_id; // 请求号，唯一标识一个请求或者响应
```

### stringcoder

#### string协议类

```c++
-- 功能 : 继承自抽象协议结构体AbstractProtocol
-- 头文件： string_coder.h
-- 类名： class StringProtocol : public AbstractProtocol
class StringProtocol : public AbstractProtocol
{
public:
    std::string info;
};

```

#### StringCoder类

```c++
class StringCoder : public AbstractCoder//继承制抽象类AbstractCoder
{      
	//将 message 对象转化为字节流，写入到 buffer
	void encode(std::vector<AbstractProtocol::s_ptr> &messages, TcpBuffer::s_ptr out_buffer)
	{
    	for (size_t i = 0; i < messages.size(); ++i)
    	{
        	std::shared_ptr<StringProtocol> msg = std::dynamic_pointer_cast<StringProtocol>(messages[i]);
        	out_buffer->writeToBuffer(msg->info.c_str(), msg->info.length());
    	}
	}
	//将 buffer 里面的字节流转换为 message 对象
	void decode(std::vector<AbstractProtocol::s_ptr> &out_messages, TcpBuffer::s_ptr buffer) 
	{
    	std::vector<char> re;
    	buffer->readFromBuffer(re, buffer->readAble());//将数据从buffer中读取到，readAble是一个递归函数，他会把所有的数据都递归获取到re，所以要做一个拼接
        std::string info;
        //进行拼接
        for (size_t i = 0; i < re.size(); ++i)
        {
            info += re[i];
        }
        std::shared_ptr<StringProtocol> msg = std::make_shared<StringProtocol>();
        msg->info = info;
        msg->m_msg_id = "123456";//服务的端口号也就是rpc请求的端口号
        out_messages.push_back(msg);//把这个输出的放到out——message里面
    }
};

```



## TCP模块

### 地址模块

#### 抽象地址类

```c++
-- 功能 : 提供地址结构
-- 头文件： net_addr.h
-- 类名： NetAddr
class NetAddr
{
public:
	typedef std::shared_ptr<NetAddr> s_ptr;
	virtual sockaddr *getSockAddr() = 0;
	virtual socklen_t getSockLen() = 0;
	virtual int getFamily() = 0;
	virtual std::string toString() = 0;
	virtual bool checkValid() = 0; // 检测合法性
};
```

#### IPNetAddr

```c++
-- 功能 : 封装IP地址
-- 头文件： net_addr.h
-- 类名： class IPNetAddr : public NetAddr
-- 变量：
1 std::string m_ip;   // ip
2 uint16_t m_port{0}; // 端口号
3 sockaddr_in m_addr; //地址信息包含端口号和ip
-- 方法
public:静态
	static bool CheckValid(const std::string &addr);
public:
	//构造函数
	IPNetAddr(const std::string &ip, uint16_t port);
	IPNetAddr(const std::string &addr);
	IPNetAddr(sockaddr_in addr);
	// 获取sock地址
	sockaddr *getSockAddr();
	// 获取sock长度
	socklen_t getSockLen();
	// 获取协议族
	int getFamily();
	// 转换成字符串返回ip 和端口
	std::string toString();
	// 检查sockaddr是否有效
	bool checkValid()；
```

### tcp数据缓冲池

![img](https://img-blog.csdnimg.cn/5ab983c0debe45a39986aa94d292cd09.png)

```c++
-- 功能 : TCP缓冲区
-- 头文件： tcp_buffer.h
-- 类名： class TcpBuffer
class TcpBuffer
{
public:
    typedef std::shared_ptr<TcpBuffer> s_ptr;
    // 构造
    TcpBuffer(int size);
    ~TcpBuffer();
    // 返回可读字节数
    int readAble();
    // 返回可写的字节数
    int writeAble();
    // 返回读索引
    int readIndex();
    // 返回写索引
    int writeIndex();
    // 写到缓冲区里
    void writeToBuffer(const char *buf, int size);
    // 从缓冲区中读
    void readFromBuffer(std::vector<char> &re, int size);
    // 重定义buffer长度
    void resizeBuffer(int new_size);
    // 就是向前移动buffer，把空位取出
    void adjustBuffer();
    // 手动调整index的值
    void moveReadIndex(int size);
    void moveWriteIndex(int size);

private:
    int m_read_index{0};  // 读取位置
    int m_write_index{0}; // 写入位置
    int m_size{0};        // buffer大小

public:
    std::vector<char> m_buffer;//缓冲区
};
```

### TcpAcceptor

```c++
-- 功能 : TCPacceptor
-- 头文件： tcp_buffer.h
-- 类名： class TcpBuffer
class TcpAcceptor
{
public:
    // 你有没有发现他都会有一个智能指针
    // 我们也试试-------------------------------------------
    // typedef std::shared_ptr<TcpAcceptor> s_ptr; 我们每次用的时候都使用域作用符
    typedef std::shared_ptr<TcpAcceptor> s_ptr;
    // 构造函数，传入的是一个网络地址--------------------------
    TcpAcceptor(NetAddr::s_ptr local_addr)
    {
        if (!local_addr->checkValid())//查看传入参数是否有效//地址是否空，端口是否合理，
        {	ERRORLOG("invalid local addr %s", local_addr->toString().c_str());exit(0);}
        //获取ip的协议族
        m_family = m_local_addr->getFamily();
        //获取套接字
        m_listenfd = socket(m_family, SOCK_STREAM, 0);
        if (m_listenfd < 0)
        {	ERRORLOG("invalid listenfd %d", m_listenfd);exit(0);}
        //设置socket选项
        int val = 1;
        if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        {	ERRORLOG("setsockopt REUSEADDR error, errno=%d, error=%s", errno, strerror(errno));}
        //绑定socket
        socklen_t len = m_local_addr->getSockLen();
        if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0)
        {	ERRORLOG("bind error, errno=%d, error=%s", errno, strerror(errno));exit(0);}
        //监听listen成功返回0，错位返回 -1;
        if (listen(m_listenfd, 1000) != 0)
        {	ERRORLOG("listen error, errno=%d, error=%s", errno, strerror(errno));exit(0);}
    }//-------------------------------------------------------------
    // 析构----------------------------------------------------------
    ~TcpAcceptor();
    // accept函数我目测啊是封装了socket的accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &clien_addr_len);-----------------------------------------------------
    std::pair<int, NetAddr::s_ptr> accept()
    {
        if (m_family == AF_INET)//首先判断一下协议族;
        {
            sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            //bzero(&client_addr, sizeof(client_addr));
            socklen_t clien_addr_len = sizeof(clien_addr_len);
            int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &clien_addr_len);
            if (client_fd < 0) { ERRORLOG();}
            IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
            INFOLOG("A client have accpeted succ, peer addr [%s]", peer_addr->toString().c_str());
            //这个peer 节点是什么意思啊，兄弟
            return std::make_pair(client_fd, peer_addr);//就是把这个对端地址和他的fd存起来
        }
        else{	return std::make_pair(-1, nullptr);	}
    }
    //--------------------------------------------------------------
    //获取用于监听的fd  m_listenfd
    int getListenFd();
private:
    //---------------------------------------------------------------
    NetAddr::s_ptr m_local_addr; // 服务端监听的地址，addr -> ip:port
    int m_family{-1}; //协议族
    int m_listenfd{-1}; //监听套接字
};
```

### TcpServer

```c++
class TcpServer
{
public:
    TcpServer(NetAddr::s_ptr local_addr): m_local_addr(local_addr){init();}// 需要传一个地址指针
    ~TcpServer();
    void start()
    {
        m_io_thread_group->start(); //开启我们的io子线程
        m_main_event_loop->loop(); //在我们的loop循环阻塞
    }
private:
    void init()
    {
        //里面的m_local_addr是TcpAcceptor构造函数的参数
        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);
        //获取主线程的线程id
        m_main_event_loop = EventLoop::GetCurrentEventLoop();
        //获取io线程在我们的配置文件里面有我们自己定义的io线程数
        m_io_thread_group = new IOThreadGroup(Config::GetGlobalConfig()->m_io_threads);
        //获取用于监听的socketfd，就是acceptor里的listen_fd
        m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());
        //fd_event可读的时候就调用onacceptor回调函数
        m_listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
        //主线程将这个m_listen_fd_event添加到epoll事件里去
        m_main_event_loop->addEpollEvent(m_listen_fd_event);
        
        //添加定时器事件,主要作用是为了清理连接
        m_clear_client_timer_event = std::make_shared<TimerEvent>(5000, true, std::bind(&TcpServer::ClearClientTimerFunc, this));
        m_main_event_loop->addTimerEvent(m_clear_client_timer_event);
    }
    // 当有新客户端连接之后需要执行
    void onAccept();
    // 清除 closed 的连接
    void ClearClientTimerFunc();

private:
    TcpAcceptor::s_ptr m_acceptor; // acceptor
    NetAddr::s_ptr m_local_addr; // 本地监听地址
    EventLoop *m_main_event_loop{NULL}; // mainReactor主线程的eventloop
    IOThreadGroup *m_io_thread_group{NULL}; // subReactor 组
    FdEvent *m_listen_fd_event;//监听事件组
    int m_client_counts{0};//客户端数量
    std::set<TcpConnection::s_ptr> m_client;//客户端智能指针
    TimerEvent::s_ptr m_clear_client_timer_event;//清理客户端连接的智能指针
};
```

### Tcpconnection

![img](https://img-blog.csdnimg.cn/cf7dd453d4674353b79dc06c5a45cdd5.png)

```c++
    //tcp状态枚举字
    enum TcpState//tcp状态枚举字
    {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };
    //TCP连接类型
    enum TcpConnectionType
    {
        TcpConnectionByServer = 1, // 作为服务端使用，代表跟对端客户端的连接
        TcpConnectionByClient = 2, // 作为客户端使用，代表跟对端服务端的连接
    };
```

```c++
-- 功能 : TCP连接
-- 头文件： tcp_connection.h
-- 类名： class TcpConnection
tcp_connection连接就是去处理缓冲区数据
class TcpConnection
{
public:
    typedef std::shared_ptr<TcpConnection> s_ptr;

public:
--------------------------------------------------------
    //构造函数{eventloop ，文件描述符，缓存长度，对端地址，本地地址，tcp连接类型}
    TcpConnection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, NetAddr::s_ptr local_addr, TcpConnectionType type = TcpConnectionByServer);
    {
        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);  //读缓冲区
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size); //写缓冲区
        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();//套接字设置为非阻塞；
        m_coder = new TinyPBCoder();
        if (m_connection_type == TcpConnectionByServer)
        {
            listenRead();//监听可读事件，就是看看tcp发没发消息
        }
    }
------------------------------------------------------------
    //{ eventloop，描述符，缓冲区大小，对端地址，本地地址，连接类型}
    ~TcpConnection();
    // connnection的三个和新方法
------------------------------------------------------------
    //读取客户端发来的数据组装为RPC请求
    void onRead()
    {
        // 1. 从 socket 缓冲区，调用 系统的 read 函数读取字节 in_buffer 里面
        if (m_state != Connected)//如果没链接就报错
        {ERRORLOG("onRead error, client has already disconneced, addr[%s], clientfd[%d]");return;}
        bool is_read_all = false;
        bool is_close = false;
        while (!is_read_all)//当没有完全读完的时候就继续读
        {
            //如果server的buffer空间没了就扩容两倍
            if (m_in_buffer->writeAble() == 0)
            {m_in_buffer->resizeBuffer(2 * m_in_buffer->m_buffer.size());}
            //所读的值就是我的缓冲区能写的大小
            int read_count = m_in_buffer->writeAble();
            int write_index = m_in_buffer->writeIndex();
            int rt = read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count);
            DEBUGLOG("success read rt bytes from addr[%s], client fd[%d]");
            if (rt > 0)//如果读到的字符大于0；
            {	//写缓冲区就移动可写位置
                m_in_buffer->moveWriteIndex(rt);
                //如果可写入大小等于读到大小，就继续；
                if (rt == read_count){continue;}
                //如果可写入大小大于读到大小，就说明读完了，直接break；
                else if (rt < read_count)
                {is_read_all = true;break;}
            }
            //另外两个一个是出错，一个是全读完都break
            else if (rt == 0)
            {is_close = true;break;}
            else if (rt == -1 && errno == EAGAIN)
            {is_read_all = true;break;}
        }
        if (is_close)//如果关闭
        {INFOLOG("peer closed, peer addr [%s], clientfd [%d]");clear();//清理链接
         return;}//如果没全读完
        if (!is_read_all){ERRORLOG("not read all data");}
        excute();//将RPC请求作为入参，执行业务逻辑获得RPC响应-------------
    }
-----------------------------------------------------------------    
    //将RPC请求作为入参，执行业务逻辑获得RPC响应
    void excute()
    {
    	//作为服务端使用，代表跟对端客户端的连接
        if (m_connection_type == TcpConnectionByServer)
        {
            // 将 RPC 请求执行业务逻辑，获取 RPC 响应, 再把 RPC 响应发送回去
            std::vector<AbstractProtocol::s_ptr> result;
            m_coder->decode(result, m_in_buffer);
            for (size_t i = 0; i < result.size(); ++i)
            {
                // 1. 针对每一个请求，调用 rpc 方法，获取响应 message
                // 2. 将响应 message 放入到发送缓冲区，监听可写事件回包
                INFOLOG("success get request[%s] from client[%s]");
                std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
                //tcpconnection和dispatch结合起来了
                RpcDispatcher::GetRpcDispatcher()->dispatch(result[i], message, this);
            }
        }
        else//作为客户端
        {
            // 从 buffer 里 decode 得到 message 对象, 执行其回调
            std::vector<AbstractProtocol::s_ptr> result;
            m_coder->decode(result, m_in_buffer);
            for (size_t i = 0; i < result.size(); ++i)
            {
                std::string msg_id = result[i]->m_msg_id;
                auto it = m_read_dones.find(msg_id);
                if (it != m_read_dones.end())
                {
                    it->second(result[i]);
                    m_read_dones.erase(it);
                }
            }
        }
    }
-------------------------------------------------------------
    //将rpc响应发送给客户端
    void onWrite()
	{
        // 将当前 m_out_buffer 里面的数据全部发送给 client
    	//如果没连接就报错
        if (m_state != Connected)
        {ERRORLOG("onWrite error, client has already disconneced, addr[%s], clientfd[%d]");return;}
    	//作为客户端使用，代表跟对端服务端的连接
        if (m_connection_type == TcpConnectionByClient)
        {
            // 1. 将 message encode 得到字节流
            // 2. 将字节流入到 buffer 里面，然后全部发送
            std::vector<AbstractProtocol::s_ptr> messages;
            for (size_t i = 0; i < m_write_dones.size(); ++i)
            {messages.push_back(m_write_dones[i].first);}
            m_coder->encode(messages, m_out_buffer);
        }
        bool is_write_all = false;
        while (true)
        {
            if (m_out_buffer->readAble() == 0)//如果没有数据发送
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            int write_size = m_out_buffer->readAble(); //可以读取的大小
            int read_index = m_out_buffer->readIndex(); //读取的位置
            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size);
            if (rt >= write_size)//如果发送的数据大小大于可发数据
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            if (rt == -1 && errno == EAGAIN)
            {	// 发送缓冲区已满，不能再发送了。
             	// 这种情况我们等下次 fd 可写的时候再次发送数据即可
          		ERRORLOG("write data error, errno==EAGIN and rt == -1");
                break;
            }
        }
        if (is_write_all)//如果已经把缓冲区都读完了
        {
            m_fd_event->cancle(FdEvent::OUT_EVENT);//取消可写事件的监听
            m_event_loop->addEpollEvent(m_fd_event);//更新fd
        }
		//如果是tcpclient//不太懂
        if (m_connection_type == TcpConnectionByClient)
        {
            for (size_t i = 0; i < m_write_dones.size(); ++i)
            {
                m_write_dones[i].second(m_write_dones[i].first);
            }
            m_write_dones.clear();
        }
	}
-------------------------------------------------------
    // 设置tcp状态
    void setState(const TcpState state);
    TcpState getState();
    // 清理连接
    void clear()
    {
        // 处理一些关闭连接后的清理动作
        //判断状态
        if (m_state == Closed){return;}
        m_fd_event->cancle(FdEvent::IN_EVENT);
        m_fd_event->cancle(FdEvent::OUT_EVENT);
        //将fd在eventloop里删除掉
        m_event_loop->deleteEpollEvent(m_fd_event);
        m_state = Closed;
    }
    // 获取fd
    int getFd();
    // 服务器主动关闭连接
    void shutdown();
    // 设置连接类型
    void setConnectionType(TcpConnectionType type);
    // 启动监听可写事件
    void listenWrite()
    {
        //如果是读事件就调用Onwrite
        m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }
    // 启动监听可读事件
    void listenRead()
    {
        //如果是写事件就调用onRead
        m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }
	
    void pushSendMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_write_dones.push_back(std::make_pair(message, done));
    }

    void pushReadMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done);
    {
        m_read_dones.insert(std::make_pair(msg_id, done));
    }
	//获取地址
    NetAddr::s_ptr getLocalAddr();
    NetAddr::s_ptr getPeerAddr();
	//回放？
    void reply(std::vector<AbstractProtocol::s_ptr> &replay_messages)
    {
        m_coder->encode(replay_messages, m_out_buffer);
        listenWrite();
    }

private:
    /// 我们定义的io线程其实就是去创建一个线程然后去调用在这个eventloop；
    EventLoop *m_event_loop{NULL}; // 代表持有该连接的 IO 线程//这个io线程就是我们event——loop
    NetAddr::s_ptr m_local_addr; // 本地地址
    NetAddr::s_ptr m_peer_addr;  // 对端服务器的地址
    TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
    TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区
    FdEvent *m_fd_event{NULL};//epollevent；
    AbstractCoder *m_coder{NULL};//抽象解码类
    TcpState m_state;//tcp状态
    int m_fd{0};

    TcpConnectionType m_connection_type{TcpConnectionByServer};
    // std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>
    std::vector<std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>> m_write_dones; // 保存写入回调函数
    // key 为 msg_id
    std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_dones; // 保存我们的回调函数
};
```

TCPclient

```c++
-- 功能 : TcpClient
-- 头文件： tcp_client.h
-- 类名： class TcpClient
class TcpClient
{
public:
    //智能指针
    typedef std::shared_ptr<TcpClient> s_ptr;
    TcpClient(NetAddr::s_ptr peer_addr):m_peer_addr(peer_addr)
    {
        //获取当前事件循环
        m_event_loop = EventLoop::GetCurrentEventLoop();
        //创建socket
        m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0);
        if (m_fd < 0)
        {ERRORLOG("TcpClient::TcpClient() error, failed to create fd");return;}
        //获取fdevent，没有的话，那边会new一个
        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(m_fd);
        //设置非阻塞
        m_fd_event->setNonBlock();
        //创建socket连接
        m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, peer_addr, nullptr, TcpConnectionByClient);
        //默认使用客户端
        m_connection->setConnectionType(TcpConnectionByClient);//默认使用客户端
    }
    ~TcpClient();
-------------------------------------------------------------------------------    
    // 异步的进行 conenct
    // 如果 connect 完成，done 回调函数会被执行
    void connect(std::function<void()> done)
    {
        // connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    	// 客户端connect （fd，服务端地址就是sockaddr_in，长度）
        int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen()); 
        if (rt == 0) //连接成功
        {
            DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
            m_connection->setState(Connected);//设置连接状态
            initLocalAddr();//获取fd所绑定的四元组 可以是get 存到成员变量m_local_addr中
            //执行回调函数
            if (done)
            {done();}
        }
        else if (rt == -1)//说明已经连接过了，或者出现错误了，如果errno == einprogress 那就说明他是
        {
            //刚好我设置的非阻塞，调用connect接口不能立马建立连接，所以就会报这个错误。
            if (errno == EINPROGRESS)
            {
                // epoll 监听可写事件，然后判断错误码
                //done是回调函数，捕获的是回调函数和this指针
                m_fd_event->listen(FdEvent::OUT_EVENT,[this, done]()
                {
                    int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
                    if ((rt < 0 && errno == EISCONN) || (rt == 0))//已被连接
                    {
                        DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
                        initLocalAddr();
                        m_connection->setState(Connected);
                    }
                    else//如果不是已被连接
                    {
                        if (errno == ECONNREFUSED)//拒绝连接
                        {
                            m_connect_error_code = ERROR_PEER_CLOSED;
                            m_connect_error_info = "connect refused";
                        }
                        else//否则就是连接失败，我们也不知道原因
                        {
                            m_connect_error_code = ERROR_FAILED_CONNECT;
                            m_connect_error_info = "connect unkonwn error, sys error ";
                        }
                        //打印日志
                        ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                        //关闭原来的套接字
                        close(m_fd);
                        //重新申请一下
                        m_fd = socket(m_peer_addr->getFamily(), SOCK_STREAM, 0);
                    }
                    // 连接完后需要去掉可写事件的监听，不然会一直触发
                    m_event_loop->deleteEpollEvent(m_fd_event);
                    DEBUGLOG("now begin to done");
                    // 如果连接完成，才会执行回调函数
                    if (done)
                    {done();}
                });
                m_event_loop->addEpollEvent(m_fd_event);//添加eventloop；
                //判断loop是否打开
                if (!m_event_loop->isLooping())
                {m_event_loop->loop();}
            }
            else//说明是其他的问题，
            {
                ERRORLOG("connect errror, errno, error";
                m_connect_error_code = ERROR_FAILED_CONNECT;
                m_connect_error_info = "connect error, sys error ";
                if (done){done();}
            }
        }
    }
---------------------------------------------------------------------------------------------------
    // 异步的发送 message//广义的字符串，可以是字符串，也可以是rpcrpc协议
    // 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
    void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);
    // 异步的读取 message
    // 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
    void readMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done);
    // 结束eventloop的方法
    void stop();
    // 获取错误码
    int getConnectErrorCode();
    // 获取错误信息
    std::string getConnectErrorInfo();
    // 获取地址
    NetAddr::s_ptr getPeerAddr();
    NetAddr::s_ptr getLocalAddr();
    // 获取fd所绑定的四元组 可以是get
    void initLocalAddr();
    // 添加定时器任务
    void addTimerEvent(TimerEvent::s_ptr timer_event);

private:
    /****************************地址模块*********************************/
    NetAddr::s_ptr m_peer_addr;  // 智能指针
    NetAddr::s_ptr m_local_addr; // 智能指针
    EventLoop *m_event_loop{NULL}; // 事件循环
    int m_fd{-1};              // 文件描述符
    FdEvent *m_fd_event{NULL}; // epoll_event
    TcpConnection::s_ptr m_connection; // tcp_connection
    int m_connect_error_code{0};      // 连接失败的错误码
    std::string m_connect_error_info; // 连接失败的错误信息
};


```

## RPC模块

RPCdispathcher 分发器

```c++
-- RPCdispathcher 分发器
-- 功能 进行RPC服务分发
-- 头文件 ：rpc_dispatcher
namespace rocket
{
    class TcpConnection;//前置依赖，防止循环依赖
    class RpcDispatcher
    {
    public:
        static RpcDispatcher *GetRpcDispatcher();

    public:

        typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;

        //调度分发器
        void dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection *connection){
            //进行类型转换 将res 和req转换成tinyPB
            //获取服务的名称：
            //res的msgid和methondname就是req的
            
        };

        //注册RPC服务方法
        void registerService(service_s_ptr service);

        //设置错误信息和错误信息码
        void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info);

    private:
        //解析服务的名字
        bool parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name);

    private:
        std::map<std::string, service_s_ptr> m_service_map;//服务注册
    };

}
```

RPCcontronller

```

```

RPCclosure

```

```

RPCchannel

```

```

RPCinterface

```

```


