## 说明
&emsp;&emsp;此工程尝试使用Qt的QGraphics系列为Ogre-next开发出一套节点编辑器，方便其他道友可以利用这个小工具进行材质、粒子编辑。分支目前分为两个：***master***和***8-6***，其中***master***预计只在7月份进行合并提交，代表本工程的最新可使用代码；***8-6***指平时开发专用分支（抢先版）。

### 才艺展示

//todo...


### 编译

1. 注意在编译**OgreSceneFormat**时，需要添加一个CmakeLists的条目

``` shell
[line 25] include_directories(${CMAKE_SOURCE_DIR}/ogre-next/Components/Hlms/Common/include)
```
//todo...


### 组成

//todo...


### Ogre下载

如果访问github速度还可以，可以通过下面的方式

``` bash 
# 下载ogre-next依赖
git clone git@github.com:OGRECave/ogre-next-deps.git

git submodule update --init --recursive

git clone git@github.com:OGRECave/ogre-next.git
```

### 备注
1. 如果INCLUDE_INNER_PARTICLE_CARD的开关进行了改变，记得清除CMakeCache.txt蛤~


### 关键结点记录
<table width="95%" cellpadding="2" cellspacing="1">
    <thead>
        <tr>
            <th></th>
            <th>时间</th>
            <th>事件</th>
            <th>备注</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>1</td>
            <td>2024-03-12</td>
            <td>首次进行git提交</td>
            <td><p>1. 已初步完成基础的卡片创建、删除<br>2. 已初步完成连接线的增删<br>3. 已具备Label控件</p></td>
        </tr>
        <tr>
            <td>2</td>
            <td>2024-03-17</td>
            <td>丰富支持的控件</td>
            <td><p>1. 支持下拉框（ComboBox）<br>2. 支持按钮（Button）<br><font color="#FF8000"><del>PS:尝试使用QGraphicsProxyWidget梭哈，但是好像有点卡..</del><br>有点误会，release模式下挺顺的orz<br>然后就能画的就画，不能画的就嘿嘿</font></p></td>
        </tr>
        <tr>
            <td>3</td>
            <td>2024-03-24</td>
            <td>丰富支持的控件以及完成数据串联</td>
            <td><p>1. 支持开关（CheckBox）<br>2. 支持线性输入（LineEdit）<br>3. 支持水平弹簧（Spacer）<br>4. 初步完成数据串联<br><font color="#FF8000">*原本打算做Undo/Redo的，渡劫失败了<br>*数据串联的时候再次感叹Qt帅的嘞~<br><del>*确实发现了锚点连接后，后面的unit不会自动更新。但是我不想让前一个unit主动触发了，因为不确定前一个cell中有多少个控制unit（虽然理论上只有一个）...</del></font></p></td>
        </tr>
        <tr>
            <td>4</td>
            <td>2024-04-04</td>
            <td>请来了另一位主角(ogre)，掌声(ohhhhhh</td>
            <td><p>1. 引入ogre-next、ogre-next-dep<br>2. 上次说的连线后自动触发一次更新改过来了</p></td>
        </tr>
        <tr>
            <td>5</td>
            <td>2024-04-14</td>
            <td>升级了控件功能，初步对PBS卡片做了拆分</td>
            <td><p>1. 输入锚点支持被多个输出锚点连接，但是只限输出锚点是绑定卡片的<br>2. 开始霍霍PBS卡片，暂且先跟组织的参数列表所支持的部分保持一致吧<br>3. 自定义数据类型可以注册并识别啦，颜色可以随机（每天换一种配色会不会开心点<br>PS: 至此控件部分还差以下内容<br>&emsp;1. 卡片支持缩进<br>&emsp;2. 向量输入的方式（Vec3f）<br></p></td>
        </tr>
        <tr>
            <td>6</td>
            <td>2024-05-12</td>
            <td>提交可扩展的单元</td>
            <td><p>1. 这次控件部分算是真告一段落（虽然还没有卡片折叠），新增的可扩展单元可以通过+/-按钮动态改变卡片单元内的成员个数<br>PS: 其实这次关键结点记录在五一就应该推上来的，五一的时候准备起手Material相关的，然后看到了自带的脚本解析模块，稍微有点道心崩碎...一直在纠结是否通过调用脚本解析的方式会更好，这样也能保持导入和导出的效果是一样的。可是最终还是选择了直接修改类成员变量或通过函数修改的方式，感觉这样在调整滑动数值的时候可以更顺一点（当然不排除以后会真香）。<br>下一步将着手Material部分，可扩展单元主要是为了实现内置脚本变量的绑定，现在控件算是交稿啦，自然要继续前进<br></p></td>
        </tr>
    </tbody>
</table>
