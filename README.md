# GrafixEditor
上海大学计算机工程与科学学院 《计算机图形学》课程项目

## 功能
* 绘制任意直线、曲线、圆、圆弧，可调整线宽线型
* 图形填充与裁剪
* 图形变换

## 开发环境
* 操作系统：Windows 10/11
* IDE：Visual Studio 2022

## 第三方依赖
* Vulkan：图形API
* glfw：图形库框架
* glm：数学库
* Dear ImGui：图形界面（立即模式）

## 配置方法
1. 在 https://vulkan.lunarg.com/ 下载Vulkan SDK并安装。
2. 在命令行输入以下git命令，将本仓库连同依赖一起下载到本地（注意：必须加**recursive**）。
```
git clone --recursive https://github.com/ycshao21/GrafixEditor.git
```
3. 打开scripts文件夹，运行 ProjectSetup-Win64.bat 文件，配置并生成Visual Studio 2022的工程项目，随后即可运行。

## 代码风格
示例代码：
```c++
namespace MyNamespace
{
    static int s_StaticVariable = 0;

    static int MyFunction()
    {
        int localVariable = 0;
        return localVariable;
    }

    struct MyStruct
    {
        char CharVariable = 'a';
        float FloatVariable = 1.0f;
    };

    class MyClass
    {
    public:
        void MyMemberFunction(int theParameter);
    private:
        int m_PrivateMemberVariable;
    };
}
```
* 命名空间、类名、函数名、结构体名以及结构体成员名采用**Pascal命名法**，即所有单词首字母大写，例如：MyNamespace
* 函数参数和局部对象采用**Camel命名法**，即：除第一个单词小写外其余单词首字母均大写，例如：localVariable
* 类保护/私有成员以“**m_**”开头，后面部分用**Pascal命名法**，例如：m_PrivateMemberVariable
* 静态变量以“**s_**”开头，后面部分用**Pascal命名法**，例如：m_StaticVariable
* 大括号“{}”全部单独成行
* 浮点数字面量写作“数值+f”，例如1.0f
* 不强制要求变量初始化
* 如果需要定义全局变量或全局函数，请尽量声明为静态（static）