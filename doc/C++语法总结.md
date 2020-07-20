## STL

(1) list 

1. 使用

```c++
// 日志输出器
class Logger {
public:
    typedef std::shared_ptr<Logger> ptr;
private:
    std::list<LogAppender::ptr> m_appenders; // Appender集合
};
```

2. 方法：

```
1. push_back       m_appenders.push_back(appender);
2. erase           m_appenders.erase(it);
```

(2) vector 语法

1. 使用

```c++
// 日志输出格式
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
public:
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem() {}
        virtual std::string format(LogEvent::ptr event) = 0;
    };
private:
    std::vector<FormatItem::ptr> m_items; 
};
```

(3) string

1. 使用

```
std::string str;
str.append(1, m_pattern[i]);
```



## 基础语法

### 多态

(1) 纯虚函数与 override 与protected使用

```c++
// 日志输出地
class LogAppender {
public:
    virtual void Log(LogLevel::Level level, LogEvent::ptr event) = 0;
protected:
    LogLevel::Level m_level;
};
```

```c++
// 控制台输出
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};
```

(2) 多态的应用

```c++
class LogFormatter {
public:
	......
public:
    class FormatItem {
    public:
        ......
        virtual void Format(std::stringstream os, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items; 
};

std::string LogFormatter::Format(LogEvent::ptr event)
{
    std::stringstream ss;
    for (auto& i : m_items) {
        i->Format(ss, event);          // 父类指针调用子类对象实现多态
    }
    return ss.str();
}
```



### C++输入输出与文件

(1) `std::ofstream `

1. 使用

```c++
// 输出到文件的Appender
class FileLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
private:
    std::string m_filename;
    std::ofstream m_filestream;
};
```

2. 方法：close
3. 方法：open
4. 方法： <<

(2)  `std::ostream`

(3) `std::stringstream`

1. 方法：str

