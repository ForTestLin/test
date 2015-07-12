#include "long_int.h"
#include <cassert>
#include <cstdio>
#include <istream>
#include <cstring>
#include <iostream>
using namespace std;

//通过一个小于等于longlong的数来构造vector
LongInt::LongInt(long long number) {//默认调用0
    //直接调用赋值的重载即可
    *this = number;
}

//复制构造函数
LongInt::LongInt(const LongInt& obj){
    *this = obj;
}


//析构函数
LongInt::~LongInt(){
    //vector没什么需要特地析构的
}

//除2 为了二分优化
LongInt LongInt::divBy2()const{
    LongInt result;
    if(*this<2)
        return 0;
    vector<int> midres;
    midres.push_back(0);
    for (int i = n.size() -1 , cur = 0; i>=0; --i) {
        int tmp = cur * _base + n[i];
        midres.push_back(tmp/2);
        cur = tmp % 2;
        ++midres[0];
    }
    int no0 = 1;
    while(not midres[no0])
        ++no0;
    result.n.clear();
    for (int i = midres[0]; i >= no0; --i) {
        result.n.push_back(midres[i]);
    }
    return result;
}

LongInt::LongInt(const string& str) {
    *this = str;
}


LongInt max(const LongInt &a, const LongInt &b){
    LongInt result(MAX(a, b));
    return result;
}
LongInt min(const LongInt &a, const LongInt &b){
    LongInt result(MIN(a,b));
    return result;
}
//取绝对值
LongInt LongInt::getABS() const{
    if(*this >= 0)
        return *this;
    else
        return getOpposite();
}
//取相反数
LongInt LongInt::getOpposite() const{
    LongInt res = *this;
    res.n.back() = -(res.n.back());
    return res;
}


//各个赋值方法
LongInt& LongInt::operator= (long long obj){
    n.clear();
    do{
        n.push_back(obj % _base);
        obj /= _base;
    }while(obj!=0);
    //除去前位0
    while(not n.back() and not n.empty())
        n.pop_back();
    if(n.empty())
        n.push_back(0);
    //如果就是0 则补回一个
    return *this;
}

//最重要的就是这个 从string来赋值
LongInt& LongInt::operator= (const string& obj){
    n.clear();
    int t=0;
    //通过字符串长度确定循环次数
    //负号占了一个长度
    bool isNegative = obj[0]=='-';
        
    unsigned long len = isNegative ?  (obj.length() - 2) / _width +  1 : (obj.length() - 1) / _width +  1;
    bool zeroCanIn = false;
    //循环次数
    for (int i=0; i < len; ++i) {
        int tail = obj.length() - i * _width;
        int begin = max(0, tail - _width);
        //读取一段出来
        sscanf(obj.substr(begin,tail - begin).c_str(), "%d" , &t);
        n.push_back(t);
    }
    //除去前位0
    while(not n.back() and not n.empty())
        n.pop_back();
    if(n.empty())
        n.push_back(0);
    //如果就是0 则补回一个
    if(isNegative)
        *this = this->getABS().getOpposite();
    return *this;
}
LongInt& LongInt::operator= (const LongInt& obj){
    n.clear();//首先清空所有的vector中元素
    for (int i = 0 ; i < obj.n.size(); ++i)
        n.push_back(obj.n[i]);
    return *this;
}

//各种算法运算符
LongInt LongInt::operator + (const LongInt& obj) const{
    LongInt result;
    result.n.clear();
    if(*this >= 0 and obj >= 0){//两个非负整数相加才是真正的相加 否则转移为减法处理
        for (int i = 0, r = 0; ; ++i) {
            if(r==0 and i>=n.size() and i>=obj.n.size())
                break;
            int t = r;
            if(i < n.size())
                t += n[i];
            if(i < obj.n.size())
                t += obj.n[i];
            result.n.push_back(t % _base);
            r = t / _base;
        }
    }else if(*this < 0 and obj < 0)
        result = (this->getABS() + this->getABS()).getOpposite();
    else if(*this < 0 and obj >= 0)
        result = obj - this->getOpposite();
    else if(*this >= 0 and obj < 0)
        result = *this - obj.getOpposite();
    return result;
    
}
LongInt LongInt::operator - (const LongInt& obj) const{
    LongInt result; result.n.clear();
    if(*this == obj)
        result = 0;
    else{
        //两个都是非负数时
        if(*this >=0 and obj >= 0){
            if(*this > obj){
                for (int i = 0, r = 0;; ++i) {
                    if(not r and i>= n.size() and i>= obj.n.size())
                        break;
                    int t = r;
                    if(i < n.size())
                        t += n[i];
                    if(i < obj.n.size())
                        t -= obj.n[i];
                    if(t < 0){
                        t += _base;  r = -1; //进位
                    }else{
                        r = 0;
                    }
                    result.n.push_back(t);
                }
                //除去前位0
                while(not result.n.back() and not result.n.empty())
                    result.n.pop_back();
                if(result.n.empty())
                    result.n.push_back(0);
                //如果就是0 则补回一个

            }else{
                result = (obj - *this).getOpposite();
            }
        }

        else if(*this>=0 and obj<0)
            result = *this + obj;
        else if(*this<0 and obj>=0)
            result = (this->getOpposite() + obj).getOpposite();
        else if(*this<0 and obj<0)
            //递归即可
            result = obj.getOpposite() - this->getOpposite();
    }
	return result;
}
LongInt LongInt::operator * (const LongInt& obj) const{
    //乘法的处理 也要考虑不同的符号
    
    
    LongInt result;
    result.n.clear();
    result = 0;
    
    LongInt a = this->getABS();
    LongInt b = obj.getABS();

    bool negative = (*this<0 and obj>0) or (*this>0 and obj<0);
    
    
    if (*this==0 or obj==0)
        return 0;
    
    for (int i=0;i<obj.n.size();++i)
    {
        LongInt midRes;//临时中间变量
        midRes.n.clear();
        if (i)//如果i大于0
            for (int j=1;j<=i;++j)
                midRes.n.push_back(0);
        unsigned long long r = 0;//中间余数
        for (int j = 0; ; ++j)
        {
            if (r == 0 && j >= n.size() )
                break;
            else{
                unsigned long long t = r;
                if (j < n.size()) t += (long long int)a.n[j] * b.n[i];
                midRes.n.push_back(t % _base);
                r = t / _base;
            }
        }
        result += midRes;
    }
    return negative ? result.getOpposite() : result;
}
//各种关系运算符
bool LongInt::operator < (const LongInt &obj) const{
    if(n.back()<0 and obj.n.back()>=0)
        return true;
    else if(n.back() >=0 and obj.n.back()<0)
        return false;
    else if(n.back() <0 and obj.n.back()<0 )
        return !(this->getABS() < obj.getABS());//递归
    else if(n.back() >= 0 and obj.n.back() >=0){
       if(n.size() != obj.n.size())
           return n.size() < obj.n.size();
        for (int k = n.size() -1 ; k>=0; --k) {
            if( n[k] != obj.n[k] )
                return n[k] < obj.n[k];
        }
    }
    return false;
}
//通过<来实现各种比较函数
bool LongInt::operator > (const LongInt &obj) const{
    return obj < (*this);
}
bool LongInt::operator <= (const LongInt &obj) const{
    return !((*this) > obj);
}
bool LongInt::operator >= (const LongInt &obj) const{
    return !((*this) < obj);
}
bool LongInt::operator == (const LongInt &obj) const{
    return !((*this)>obj or (*this)<obj);
}
bool LongInt::operator != (const LongInt &obj) const{
    return !((*this)==obj);
}
//demo的很多地方用!加一个LongInt类来判断非空
bool LongInt::operator ! () const{
    //!a 如果a是0 则返回true
    return *this == 0 ;
}
//二分探商优化 效率比短除法大大提高...
LongInt LongInt::operator / (const LongInt &obj) const{
	assert(obj!=0 && "devide zero");
    
    bool negative = (*this<0 and obj>0) or (*this>0 and obj<0);
    
    LongInt a = this->getABS() , b = obj.getABS();
    //两类特殊情况
    if(a < b)
        return 0;
    if(a == b)
        return negative ? -1 : 1;
    LongInt lft = 1, rgt = a;
    LongInt curSum = lft + a;
    LongInt middle = curSum.divBy2();
    LongInt one = 1;
    //开始二分
    while( not(a >= middle * b and a < (b + middle * b) )){
        if(middle * b < a) //探商
            lft = middle + one;
        else
            rgt = middle;
        curSum = lft + rgt;
        middle = curSum.divBy2();
    }
    return negative ? middle.getOpposite() : middle;
}

//取余数 不考虑效率的做法....
LongInt LongInt::operator % (const LongInt &obj) const{
	return (*this) - (*this/obj)*obj;
}
