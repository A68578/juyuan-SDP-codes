整体思路：
使用表维护这个流程，进行解析
可以跳转的
上面那个表
先检查reprog
如果有直接进boot mcal
10 02->50 02
再检查app
然后挂起
是否为reset
如果是reset则reset
如果不是
跳app
如果app无效则进跳mcal
最后到mainfunction
dcminit改
target go改分别融合在reprog和reset中


1.增加状态机与功能描述
2.增加备份数据功能
3.只考虑了该模块的功能，没有考虑其他模块的耦合，例如bsw模块的初始化，先做个假的接口。

