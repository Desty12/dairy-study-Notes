## **Git的使用教程**

 Git 常用的是以下 6 个命令：**git clone**、**git push**、**git add** 、**git commit**、**git checkout**、**git pull**，后面详细介绍。 

 ![img](https://www.runoob.com/wp-content/uploads/2015/02/git-command.jpg) 

**说明：**

- workspace：工作区
- staging area：暂存区/缓存区
- local repository：版本库或本地仓库
- remote repository：远程仓库

一个简单的操作步骤：

```
$ git init    		    #  初始化本地仓库
$ git add [文件名]   	  #  将文件提交到暂存区
$ git commit -m"备注信息"   			#	将文件提交到本地仓库，一般加 “-m” 表示本次提交时的备注信息

# 本地仓库与远程仓库简历链接
$ git remote add [本地仓库结点名] 远程仓库的地址（git.xxxx.xxx）
# 将本地仓库的文件push到远程仓库
$ git push [本地仓库结点名] -f     # 加“-f”表示强制提交到远程仓库

# 从远程仓库clone文件或者项目到本地仓库中
$ git clone [远程仓库地址]

```

- git init - 初始化仓库。
- git add [文件名] - 添加文件到暂存区。
- git commit - 将暂存区内容添加到仓库中。

### Git的基本概念：

- 工作区：就是你在电脑里能看到的目录。

- 暂存区：一般存放在 “.git目录下” 下的index文件（.git/index）中，所以也叫索引

- 版本库：工作区有一个隐藏目录 .Git，这个不算工作区，而是Git的版本库。

  使用git add添加一个文件的时候，会先加入到暂存区；

  通过git commit之后，暂存区的内容同步到正式的版本库。

  ![1645945114067](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645945114067.png)

  ![1645944963092](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645944963092.png)

  ```
  Git的命令窗口支持使用Linux系统命令
  比如：在Git的命令窗口使用Linux的Vi编辑器文本创建txt文件，之后使用git add将新创建的文本添加到暂存区，然后使用git commit将新创建的文本添加到版本库中步骤如下：
  1、使用linux的Vi编辑器命令创建一个hello.txt文件，并向其中输入一些内容
  	vim hello.txt
  2、将新建的hello.txt推送到暂存区中
  	命令格式：git add 文件名
  	git add -u 可以将当前托管的文件一起提交，不需要指定文件名
  		git add hello.txt
  3、使用git status命令可以看到当前暂存区的状态
  git status
  
  4、将暂存区中的文件提交到远程版本库中
  	git commit -m "提交时添加必要的信息，以方便其他的协同人员可以看到这一更新信息"
  ```

  ![1645946380738](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645946380738.png)

  当使用git commit之后，会发现再次使用git status命令时暂存区中的文件已经被提交了

  【注】

  1、commit只提交暂存区中的数据，所以需要先add，如果是文件已经add过了，可以使用-am参数直接提交

  2、如果想要修改最近一次的提交信息message，使用git commit --amend

  

  

  ### Git的全局配置

  输入用下面的命令，做一个全局配置，后续提交代码的时候会携带这些配置信息，最好用前面祖册git的邮箱

  ```
  git config --global user.name "你的名字"
  git config --global user.email "你的邮箱"
  ```

  之后在你的电脑用户目录下会出现一个.gitconfig文件，全局配置就设置好了

  ![1645944393108](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645944393108.png)

  .gitconfig里边的文件内容如下：

  ![1645944435631](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645944435631.png)

  配置后了之后，使用git config --list 可以查看配置信息

  ![1645944598223](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645944598223.png)

  ### 初始化git

  使用命令git init初始化工作区

  

  

  ### Git常见命令的一些操作

  #### 1.1 git diff

  在执行git diff之前，我们先对hello.txt文件的内容进行修改

  ![1645947450213](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645947450213.png)

  然后再执行git add和git status，就可以看见文件是一个modified状态：

  ![1645947424716](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645947424716.png)

  执行git diff --cached 可以查看暂存区中的变动（和HEAD的差别）

  如果不加 --cached参数，则是查看工作区和暂存区的差别

  #### 1.2 git reset

  ```
  git reset用于回滚暂存区中的操作，对文件进行修改：
  	git reset
  通过add添加到暂存区，然后reset，可以看到钢材暂存区的操作被取消了
  	git add hello.txt
  	...
  	git reset
  ```

  #### 1.3 git mv

  ```
  命令3： git mv 文件重命名命令
  	git mv 原文件 目标文件
  这个命令会同时修改git库和你本地的文件名
  ```

  #### 1.4 git log

  查看日志 git log，使用git log可以记录最近做过一些什么操作

  ```
  参数：
  --oneline 借鉴的查看历史
  -n 最近几次的记录
  --all 查看所有分支所有记录
  --graph 图形化方式
  ```

  ![1645948500827](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645948500827.png)

  查看最近两次的记录：

  ![1645948552356](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645948552356.png)

  查看所有分支记录：git log --all

  图形化方式显示提交信息：git log --graph

  ![1645948653854](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645948653854.png)

  

  #### 1.5 git branch

  分支是git中非常重要的一个概念，在开发时我们可以创建多个分支，每一个分支将都是相互独立的，互不影响。当分支开发完毕后，可以选择合并到主干

  ```
  git branch命令用于操作git分支：
  	git branch -v 				  看本地有多少分支
  	git branch -d 分支名  			删除分支
  ```

  

  

  #### 1.6 git checkout

  可以从主干或其他分支快速创建一个新分支或切换分支，使用git checkout命令，在checkout的时候我们可以指定一个版本号，这会版本号就是git log中查到的hash值，版本号在使用的时候可以只使用前几位，只要当前的几位能够确定唯一标识就可以。

  ```
  git checkout -b 新分支 原分支/conmmit 					创建分支
  git checkout 分支										  切换分支
  ```

  例如：使用git checkout命令创建一个基于master的分支temp

  ![1645949505743](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645949505743.png)

  使用git branch -v 可以看到 * 已经添加到分支temp上

  ![1645949543679](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645949543679.png)

  使用命令：git checnout 需要切换的分支，可以进行分支的切换

  ![1645949738658](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645949738658.png)

#### 1.7 分离头指针

当checkout一个commit的hash时，会看到如下情况

![1645953669150](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645953669150.png)

这个提示的意思是，当前状态是分离头指针，即没有分支与当前的编辑版本关联；我们可以选择提交或废弃当前的编辑。但如果不与分支关联，当切换分支的时候，当前编辑的版本将全部丢失。

分离头指针，可以用在一些尝试性的修改上，比如我想修改一些功能，但是不能确定最终是否可行，那在分离头指针的状态，如果当前编辑不可行，就直接切换分支，丢弃当前的编辑。

我们尝试修改并提交：

![1645954086472](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645954086472.png)

此时如果我们切换分支，会出现如下情况：

![1645954164454](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645954164454.png)

通过git log去查询，看不到任何与这个记录相关的东西：

![1645954296995](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645954296995.png)

如果想保留刚才的修改，就去执行创建分支的命令：

```
git branch newBranch 176269b6d328d15e3                  # 这一串字符表示临时创建的分支
如果经过这样的操作之后，这临时分支里边创建的操作会得到保留
```

#### 1.8 gitk

gitk 能够打开一个图形界面，其打开界面结果显示如下：

![1645954981537](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645954981537.png)

我们可以看到各个版本的信息，包括父子关系，提交时间。tree可以显示目录结构。在View中可以定义显示的视图

![1645955652179](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645955652179.png)

#### 1.9 git rebase

这个操作交变基操作，可以指定修改历史版本中的提交信息

git rebase -i hash值（hash选的是父亲）可以打开一个交互界面：





#### 2.0 git rm

删除一个文件git rm [文件名] 删除一个文件，会删除工作区中的文件，并把变动放在暂存区，加--cached参数，则删除暂存区中的东西，不删除工作区

![1645956257970](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645956257970.png)

删除了工作区中的hello.txt文件，但是暂存区中保留了删除信息，如果此时进行commit操作，则new1分支中的hello.txt文件将会被真删除。

![1645956470746](C:\Users\12323\AppData\Roaming\Typora\typora-user-images\1645956470746.png)

#### 2.1 git stash

如果我的文件，有一部分新功能A在暂存区，但临时增加了别的任务B，需要在之前的版本上修改，而我的新功能A还没有开发完，这时我们可以使用git stash命令来保存暂存区，此时文件会恢复到暂存区之前的样子。







#### 2.3 使用git push将文件添加到远程仓库的操作

**一 操作准备条件：**

  git远程仓库已经建好了，本地文件已经存在了，现在要将本地代码推到git远程仓库保存。

解决办法如下：

1、（先进入项目文件夹）通过命令 git init 把这个目录变成git可以管理的仓库

git init

2、把文件添加到版本库中，使用命令 git add .添加到暂存区里面去，不要忘记后面的小数点“.”，意为添加文件夹下的所有文件
git add .

3、用命令 git commit告诉Git，把文件提交到仓库。引号内为提交说明：*git commit -m 'first commit'*

 ![img](https://img2018.cnblogs.com/blog/1238466/201812/1238466-20181223125250898-2018547309.png)

4、关联到远程库
git remote add origin 你的远程库地址

5、获取远程库与本地同步合并（如果远程库不为空必须做这一步，否则后面的提交会失败）
git pull --rebase origin master

6、把本地库的内容推送到远程，使用 git push命令，实际上是把当前分支master推送到远程。执行此命令后会要求输入用户名、密码，验证通过后即开始上传。
git push -u origin master

![img](https://img2018.cnblogs.com/blog/1238466/201812/1238466-20181223125452477-1918999174.png)

![img](https://img2018.cnblogs.com/blog/1238466/201812/1238466-20181223130040332-1998985234.png)







