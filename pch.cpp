#include "pch.h"

#include <QApplication>
#include <QTranslator>
#include <QDir>

#include "Geometry/nctriaopenmesh.h"

//√‹π∑
static const char AppName[] = "KWBender_2020";
static const char PassWord[] = "23dd26f5b2e47a99d971f5c2fb6c0ec0";

//◊¢≤·√‹π∑
static const char Encode_AppName[] = "Encode_KWBender_2020";
static const char Encode_PassWord[] = "29d3db237a4593fc91e4d3a91855bc8b";

static QThread *coreThread = nullptr;

NCTriaMeshSPtr g_pMesh          = nullptr;
NCKnifeSetSPtr g_pKnifeSet      = nullptr;
ToolDataSPtr   g_pBlank         = nullptr;

vector<NCPath> g_paths;

CVector3d XDIR = CVector3d(1, 0, 0);
CVector3d YDIR = CVector3d(0, 1, 0);
CVector3d ZDIR = CVector3d(0, 0, 1);


Initial::Initial()
{
    g_pMesh.reset(new NCTriaOpenMesh());
    g_pKnifeSet.reset(new NCToolSet());

}

Initial::~Initial()
{

}

bool SwitchLanguage(LA nLa)
{
    static QString strTranFile[2] = {
                QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator() + QString("KWBender_zh_CN.qm"),
                QString("NCCutting_en_US.qm")
            };

    QTranslator *pNewTranslator = new QTranslator(qApp);

    bool res = false;

    if (pNewTranslator->load(strTranFile[nLa]))
    {
        res = qApp->installTranslator(pNewTranslator);

//        ///StingTable∑≠“Î
//        NCStringTable::Translate();
//        if(g_pWarn)
//        {
//            g_pWarn->Init();
//        }
    }
    else
    {
        if (pNewTranslator)
        {
            delete pNewTranslator;
            pNewTranslator = nullptr;
        }
    }
    return res;
}

bool check(const string &fileName, const string &fileSuffix)
{
    string::size_type pos = fileName.find_last_of('.');
    if(pos == string::npos)
    {
        return false;
    }

    string suffix = fileName.substr(pos+1);
    transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);
    return suffix == fileSuffix;
}
