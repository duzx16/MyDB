* 现有模块的CMakeList我已经写好了，后续添加的模块只要照着写就可以了
* 为了方便我把BufPageManager和FileManager重构了一下，变成了单例模式，通过getInstance方法获取。
* 这门课大作业的模块划分是仿照CS346的项目https://web.stanford.edu/class/cs346/2015/redbase.html ,所以我的记录管理模块的接口也是仿照CS346写的，如果没有好的想法可以参照链接里的说明。
* 索引用到的B+ Tree有已经实现好的C++模板，参见https://panthema.net/2007/stx-btree/ ，当然你要手撸我也没意见。
* 如果没有特殊情况以后还是分branch写吧