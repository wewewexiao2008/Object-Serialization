VS2019 windows64
debug x64 模式编译

test包含了对int,float等基本类型
string
list,vector及其嵌套(set同理则未包含)
pair,map及与其他STL容器的嵌套
给定UserDefinedType
的测试

bonus1：binary mode of xml serialization
1) 对字符串存储使用base64编码 使得某些不可见字符可以保存
2) XMLserial.h中 默认 is_bin = false，在xml test#10后设置为
ture开启该模式

bonus2: smart ptr
在binary模块中实现