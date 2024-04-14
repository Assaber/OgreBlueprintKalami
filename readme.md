## 说明
&emsp;&emsp;此工程尝试使用Qt的QGraphics系列为Ogre-next开发出一套节点编辑器，方便其他道友可以利用这个小工具进行材质、粒子编辑。分支目前分为两个：***master***和***8-6***，其中***master***预计只在7月份进行合并提交，代表本工程的最新可使用代码；***8-6***指平时开发专用分支（抢先版）。

### 才艺展示

//todo...


### 编译

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
    </tbody>
</table>
