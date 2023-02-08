// 用药信息
struct DRUG
{
    long long STARTDATE; // 用药开始时间 2175-06-11 00:00:00
    long long ENDDATE;   // 用药结束时间 2175-06-12 00:00:00
    char *PROD_STRENGTH; // 用药强度, 例如:1mg Capsule
    long long GSN;             // 通用序列号
    long long NDC;       // 国家药品编码
    int DIFF;            // 开始与结束时间之间的差值(单位:小时)
};

// 医嘱
struct NOTE
{
    char *TEXT;     // 医嘱文本
    long long DATE; // 时间, 当天最后一个小时的时间戳
};

// 病人基本信息
struct StaticInformation
{
    int Age;
    int Gender;
    int Util1;
    int Util2;
    long long HospAdmiTime;
    long long begintime;
    long long endtime;
};

// 需要提取的特征
struct Feature_ID
{
    char name[256];
    int ITEMID[256];
};

// 取出的特征ID与数值
struct Feature
{
    int ITEMID;
    double value;
    long long time;
};

struct H_ID_node
{
    int HADM_ID;
    int SUBJECT_ID;
    struct H_ID_node *left;
    struct H_ID_node *right;
};