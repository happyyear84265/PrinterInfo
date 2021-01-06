// testprinterInfo.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <vector>
using namespace std;
#ifdef _UNICODE
#else
HANDLE hPrinter;
int main(int argc, char* argv[])
{

    long bufsize = 0x100;
    char prnname[100] = "NPIB1588C (HP LaserJet Professional P 1102w)"; //Guestgulkans printer name

   
    if (OpenPrinter(prnname, &hPrinter, NULL) == 0)
    {
        cout << "OpenPrinter call failed" << endl;

        return 0;
    }
    DWORD numJobs=0;
    JOB_INFO_4* pJobInfo = 0;
    DWORD bytesNeeded = 0, jobsReturned = 0;
    while (numJobs ==0)
    {
        DWORD dwBufsize = 0;
        GetPrinter(hPrinter, 2, NULL, 0, &dwBufsize);

        PRINTER_INFO_2* pinfo = (PRINTER_INFO_2*)malloc(dwBufsize);
        long result = GetPrinter(hPrinter, 2, (LPBYTE)pinfo, dwBufsize, &dwBufsize);
        numJobs = pinfo->cJobs;
        free(pinfo);//free now
        if (numJobs > 0)
        {        //Get info about jobs in queue.
            EnumJobs(hPrinter, 0, numJobs, 4, (LPBYTE)pJobInfo, 0, &bytesNeeded, &jobsReturned);
            pJobInfo = (JOB_INFO_4*)malloc(bytesNeeded);
            EnumJobs(hPrinter, 0, numJobs, 4, (LPBYTE)pJobInfo, bytesNeeded, &bytesNeeded, &jobsReturned);
        }
    }
    //Loop and delete each waiting job
    for (int count = 0; count < jobsReturned; count++)
    {
        cout << "Deleting JobID  " << pJobInfo[count].JobId;
        if (SetJob(hPrinter, pJobInfo[count].JobId, 0, NULL, JOB_CONTROL_DELETE) != 0)
        {
            cout << "...... Deleted OK" << endl;
        }
        else
        {
            cout << "...... Failed to Delete" << endl;
        }
    }

    free(pJobInfo);//free now


    DOC_INFO_1 DocInfo = { 0 };


    DocInfo.pOutputFile = NULL;
    char re[5] = _T("text");
    DocInfo.pDatatype = re;//text format


    HANDLE fHandle;
    WIN32_FIND_DATA fData = { 0 };


    string dirName("D:\\some_folder\\"); //The folder where the text files are kept

    fHandle = FindFirstFile(_T((dirName + "*.txt").c_str()), &fData);

    if (fHandle != INVALID_HANDLE_VALUE)
    {

        ifstream f;
        string str;
        do
        {

            f.open((dirName + fData.cFileName).c_str());

            if (f.is_open())
            {
                string ss;
                while (getline(f, ss))
                {

                    ss += "\n";
                    str += ss;
                }

                f.close();

            }

            //Start the print
            DocInfo.pDocName = _T(fData.cFileName);//Set document title to the  current filename (without path)
            StartDocPrinter(hPrinter, 1, (LPBYTE)&DocInfo);
            StartPagePrinter(hPrinter);

            DWORD dwBytesWritten = 0;
            if (str.length() > 0)
            {
                WritePrinter(hPrinter, (LPVOID)str.c_str(), str.length(), &dwBytesWritten);
            }

            //Finish printing
            str.clear();
            EndPagePrinter(hPrinter);
            EndDocPrinter(hPrinter);

            DeleteFile((dirName + fData.cFileName).c_str());

        } while (FindNextFileA(fHandle, &fData) != FALSE);

        FindClose(fHandle);


    }//If valid find handle


    //Finished with the printer
    ClosePrinter(hPrinter);

    return 0;

}
#endif
// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
