按以下步骤编译运行代码：

**提取特征列表文件：**

在开始提取前需要准备一个文本文件提供需要提取的特征名称与特征ID，文件中内容如下：

```
HR,211
HR,220045
O2Sat,50817
O2Sat,220277
......
```

其中每行分别表示一个特征，特征名在前，ID在后，中间通过`,`分隔，名称相同的特征在提取后会被合并到同一列，具体内容可参考`featureslist_sample.txt`文件。



**配置数据集路径、索引路径、输出路径及特征列表文件路径：**

本程序需要通过索引文件查找所需提取数据的位置，请先下载并解压索引文件，下载地址：[百度网盘    提取码:2aej](https://pan.baidu.com/s/1571AxIPC-ovCy_O5NQKMWw )

在准备好索引文件后在`consts.h`文件中写入索引文件、特征列表、输出目录以及MIMIC III数据集所在位置，示例如下：

```c
char FEATURES_LIST[] = "D:/MIMIC/3/featureslist.txt";	//特征列表文件位置
char OUTPUT_DIR[] = "D:/MIMIC/3/outputtest/";			//输出路径
char data_path[] = "F:/MIMIC/mimiciii data/";			//数据集所在目录
char index_path[] = "F:/MIMIC/mimiciii index/";			//索引所在目录
```



**编译：**

使用gcc编译：

```
gcc main.c -lm -o xxx
```

编译后生成名为xxx的可执行文件，运行该文件即可开始提取。

开始提取后会在输出路径下生成以病人HADM_ID为文件名的csv文件。

