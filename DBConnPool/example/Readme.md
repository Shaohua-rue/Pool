测试使用user表创建如下

```mysql
CREATE TABLE user (
    id INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(50),
    age INT(11),
    sex ENUM('male', 'female')
);
```
