#include <iostream>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>
#include <chrono>
#include <random>
#include <limits>
#include <atomic>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>
#include <filesystem>
#include <map>
#include <tuple>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
using namespace std;
void bubbleSort(vector<int>&v);
void selectionSort(vector<int>&v);
void insertionSort(vector<int>&v);
void merge(vector<int>&v,int l,int m,int r);
void mergeSort(vector<int>&v,int l,int r);
int medianOfThree(const vector<int>&v, int low, int high);
void quickSort(vector<int>&v,int low,int high);
void heapSort(vector<int>&v);
void heap(vector<int>&v,int n,int i);
void monitor(string name, function<void()> func);
void generateRandomArray(vector<int>&v,int s);
void generateSortedArray(vector<int>&v,int n);
void generateReverseArray(vector<int>&v,int n);
void generateAlmostSortedArray(vector<int>&v,int n);
void generateFewUniqueArray(vector<int>&v,int n);
string getCurrentTime();
string getFilenameTimestamp();
size_t getCurrentRAM();
void computeAverages();
struct Result {
    string name;
    long long time_ns;
    long long time_ms;
    size_t memory_bytes;
};
mutex mtx; vector<Result> results;
string formatDuration(long long ms) {
    long long hours = ms / (1000 * 60 * 60);
    ms %= (1000 * 60 * 60);
    long long minutes = ms / (1000 * 60);
    ms %= (1000 * 60);
    long long seconds = ms / 1000;
    long long milliseconds = ms % 1000;
    stringstream ss;
    if (hours > 0) ss << hours << "h ";
    if (minutes > 0) ss << minutes << "m ";
    if (seconds > 0) ss << seconds << "s ";
    if (milliseconds > 0 || (hours == 0 && minutes == 0 && seconds == 0)) ss << milliseconds << "ms";
    return ss.str();
}
string formatPrecise(long long ns)
{
    stringstream ss;
    if(ns < 1000)
        ss << ns << " ns";
    else if(ns < 1000000)
        ss << fixed << setprecision(3) << ns/1000.0 << " us";
    else if(ns < 1000000000)
        ss << fixed << setprecision(3) << ns/1000000.0 << " ms";
    else
        ss << fixed << setprecision(3) << ns/1000000000.0 << " s";
    return ss.str();
}
string getCurrentTime()
{
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    tm local_tm;
    localtime_s(&local_tm, &now_time);

    stringstream ss;
    ss << put_time(&local_tm, "%Y-%m-%d %H:%M:%S");

    return ss.str();
}
string getFilenameTimestamp()
{
    auto now = chrono::system_clock::now();
    time_t now_time = chrono::system_clock::to_time_t(now);

    tm local_tm;
    localtime_s(&local_tm, &now_time);

    stringstream ss;
    ss << put_time(&local_tm, "%d.%m.%Y_%H-%M-%S");
    return ss.str();
}
size_t getCurrentRAM()
{
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(),&pmc,sizeof(pmc));
    return pmc.WorkingSetSize;
}
void monitor(string name, function<void()> func) 
{
    atomic<bool> ver(false);
    thread monitorThread([&](){
        while(!ver) 
        {
            {
            lock_guard<mutex> lock(mtx);
            cout<<"[Ruleaza] "<<name<<" la ["<<getCurrentTime()<<"]\n";
            }
            this_thread::sleep_for(chrono::seconds(5));
        }
    });
    {
        lock_guard<mutex> lock(mtx);
        cout<<"[Pornit] "<<name<<" la ["<<getCurrentTime()<<"]\n";
    }
    size_t mem_before=getCurrentRAM();
    auto start=chrono::high_resolution_clock::now();
    func();
    auto end=chrono::high_resolution_clock::now();
    size_t mem_after=getCurrentRAM();
    size_t mem_used=0;
    if(mem_after>mem_before)
        mem_used=mem_after-mem_before;
    ver=true;
    monitorThread.join();
    auto duration_ns=chrono::duration_cast<chrono::nanoseconds>(end - start);
    long long ns = duration_ns.count();
    long long ms = ns / 1000000;
    {
        lock_guard<mutex> lock(mtx);
        results.push_back({name, ns, ms, mem_used});
        cout<<"[Finalizat] "<<name<<" la ["<<getCurrentTime()<<"] a durat "<<formatPrecise(ns)<<" | RAM +"<<mem_used/1024<<" KB\n";
    }
}
void selectionSort(vector<int>&v) 
{
    int n=v.size();
    for(int i=0; i<n-1;i++) 
    {
        for(int j=i+1;j<n;j++) 
        {
            if(v[i]>v[j]) 
            {
                swap(v[i],v[j]);
            }
        }
    }
}
void bubbleSort(vector<int>&v)
{
    int n=v.size();
    bool ver=false;
    while(!ver)
    {
        ver=true;
        for(int i=0;i<n-1;i++)
        {
            if(v[i]>v[i+1])
            {
                swap(v[i],v[i+1]);
                ver=false;
            }
        }
    }
}
void insertionSort(vector<int>&v)
{
    int n=v.size();
    for(int i=1;i<n;i++)
    {
        int k=v[i];
        int j=i-1;
        while(j>=0 && v[j]>k)
        {
            v[j+1]=v[j];
            j--;
        }
        v[j+1]=k;
    }
}
void merge(vector<int>&v,int l,int m,int r) 
{
    int i=l;
    int j=m+1;
    vector<int> temp;
    while(i<=m&&j<=r) 
    {
        if(v[i]<=v[j]) 
        {
            temp.push_back(v[i]);
            i++;
        } 
        else 
        {
            temp.push_back(v[j]);
            j++;
        }
    }

    while(i<=m) 
    {
        temp.push_back(v[i]);
        i++;
    }

    while(j<=r) 
    {
        temp.push_back(v[j]);
        j++;
    }

    for(int k =l;k<=r;k++) 
    {
        v[k]=temp[k-l];
    }
}
void mergeSort(vector<int>&v,int l,int r) 
{
    if(l<r) 
    {
        int m=l+(r-l)/2;
        mergeSort(v,l,m);
        mergeSort(v,m+1,r);
        merge(v,l,m,r);
    }
}
int medianOfThree(const vector<int>&v, int low, int high)
{
    int mid = low + (high - low) / 2;
    int a = v[low], b = v[mid], c = v[high];
    
    if((a <= b && b <= c) || (c <= b && b <= a))
        return b;
    else if((b <= a && a <= c) || (c <= a && a <= b))
        return a;
    else
        return c;
}
void quickSort(vector<int>&v,int low,int high) 
{
    while(low<high) 
    {
        if(high - low < 16)
        {
            for(int i = low + 1; i <= high; i++)
            {
                int key = v[i];
                int j = i - 1;
                while(j >= low && v[j] > key)
                {
                    v[j + 1] = v[j];
                    j--;
                }
                v[j + 1] = key;
            }
            return;
        }
        
        int pivot = medianOfThree(v, low, high);
        
        int lt=low;
        int i=low;
        int gt=high;
        
        while(i<=gt) 
        {
            if(v[i]<pivot) 
            {
                swap(v[i],v[lt]);
                lt++;
                i++;
            }
            else if(v[i]>pivot) 
            {
                swap(v[i],v[gt]);
                gt--;
            }
            else 
            {
                i++;
            }
        }
        
        if(lt-low < high-gt)
        {
            quickSort(v, low, lt-1);
            low = gt+1;
        }
        else
        {
            quickSort(v, gt+1, high);
            high = lt-1;
        }
    }
}   
void heapSort(vector<int>&v) 
{
    int n=v.size();
    for(int i=n/2-1;i>=0;i--) 
    {
        heap(v,n,i);
    }
    for(int i=n-1;i>0;i--) 
    {
        swap(v[0],v[i]);
        heap(v,i,0);
    }
}
void heap(vector<int>&v,int n,int i) 
{
    int largest=i;
    int l=2*i+1;
    int r=2*i+2;
    if(l<n && v[l]>v[largest]) 
    {
        largest=l;
    }
    if(r<n && v[r]>v[largest]) 
    {
        largest=r;
    }
    if(largest!=i) 
    {
        swap(v[i],v[largest]);
        heap(v,n,largest);
    }
}
void generateRandomArray(vector<int>&v,int s) 
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1,1000000);
    for(int i=0;i<s;i++) 
    {
        v.push_back(dis(gen));
    }
}
void generateSortedArray(vector<int>&v,int n)
{
    v.resize(n);
    for(int i=0;i<n;i++)
    {
        v[i]=i;
    }
}
void generateReverseArray(vector<int>&v,int n)
{
    v.resize(n);
    for(int i=0;i<n;i++)
    {
        v[i]=n-i;
    }
}
void generateAlmostSortedArray(vector<int>&v,int n)
{
    v.resize(n);
    for(int i=0;i<n;i++)
    {
        v[i]=i;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0,n-1);
    int swaps=max(1,n*5/100);
    for(int i=0;i<swaps;i++)
    {
        int a=dis(gen);
        int b=dis(gen);
        swap(v[a],v[b]);
    }
}
void generateFewUniqueArray(vector<int>&v,int n)
{
    v.resize(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1,5);
    for(int i=0;i<n;i++)
    {
        v[i]=dis(gen);
    }
}
void computeAverages()
{
    namespace fs = std::filesystem;

    map<tuple<string,string,int>, vector<double>> timeNsMap;
    map<tuple<string,string,int>, vector<double>> timeMap;
    map<tuple<string,string,int>, vector<double>> ramMap;

    for(const auto& entry : fs::directory_iterator(fs::current_path()))
    {
        if(!entry.is_regular_file())
        {
            continue;
        }

        const string filename = entry.path().filename().string();
        const string prefix = "results_";
        const string suffix = ".csv";

        if(filename.rfind(prefix, 0) != 0)
        {
            continue;
        }
        if(filename.size() < suffix.size() || filename.substr(filename.size() - suffix.size()) != suffix)
        {
            continue;
        }

        const size_t datasetStart = prefix.size();
        const size_t nPos = filename.find("_n", datasetStart);
        if(nPos == string::npos)
        {
            continue;
        }

        const string datasetType = filename.substr(datasetStart, nPos - datasetStart);
        const size_t sizeStart = nPos + 2;
        const size_t sizeEnd = filename.find('_', sizeStart);
        if(sizeEnd == string::npos || sizeEnd <= sizeStart)
        {
            continue;
        }

        int datasetSize = 0;
        try
        {
            datasetSize = stoi(filename.substr(sizeStart, sizeEnd - sizeStart));
        }
        catch(...)
        {
            continue;
        }

        ifstream fin(entry.path());
        if(!fin.is_open())
        {
            continue;
        }

        string line;
        getline(fin, line);

        while(getline(fin, line))
        {
            if(line.empty())
            {
                continue;
            }

            stringstream ss(line);
            vector<string> fields;
            string field;
            while(getline(ss, field, ','))
            {
                fields.push_back(field);
            }

            if(fields.size() < 6)
            {
                continue;
            }

            const string algorithm = fields[0];
            double timeNs = 0.0;
            double timeMs = 0.0;
            double ramKb = 0.0;

            try
            {
                if(fields.size() >= 7)
                {
                    timeNs = stod(fields[1]);
                    timeMs = timeNs / 1e6;
                    ramKb = stod(fields[5]);
                }
                else
                {
                    timeMs = stod(fields[1]);
                    timeNs = timeMs * 1e6;
                    ramKb = stod(fields[4]);
                }
            }
            catch(...)
            {
                continue;
            }

            tuple<string,string,int> key = make_tuple(algorithm, datasetType, datasetSize);
            timeNsMap[key].push_back(timeNs);
            timeMap[key].push_back(timeMs);
            ramMap[key].push_back(ramKb);
        }
    }

    ofstream fout("averages.csv");
    fout << "Algoritm,TipDate,Dimensiune,Timp_mediu_ns,Timp_mediu_ms,RAM_mediu_KB,Esantioane\n";

    for(const auto& entry : timeMap)
    {
        const auto& key = entry.first;
        const auto& timesNs = timeNsMap[key];
        const auto& times = entry.second;
        const auto& rams = ramMap[key];

        if(timesNs.empty() || times.empty() || rams.empty())
        {
            continue;
        }

        double sumTimeNs = 0.0;
        for(double value : timesNs)
        {
            sumTimeNs += value;
        }

        double sumTime = 0.0;
        for(double value : times)
        {
            sumTime += value;
        }

        double sumRam = 0.0;
        for(double value : rams)
        {
            sumRam += value;
        }

           const double avgTimeNs = sumTimeNs / static_cast<double>(timesNs.size());
        const double avgTime = sumTime / static_cast<double>(times.size());
        const double avgRam = sumRam / static_cast<double>(rams.size());

        fout << get<0>(key) << ","
             << get<1>(key) << ","
             << get<2>(key) << ","
               << fixed << setprecision(6) << avgTimeNs << ","
             << fixed << setprecision(6) << avgTime << ","
             << fixed << setprecision(6) << avgRam << ","
             << times.size() << "\n";
    }

    cout << "Mediile au fost calculate cu succes si salvate in averages.csv\n";
}
int main()
{
    vector<int> v;
    int sursa;
    int tip;
    int s;
    string datasetName;
    cout<<"Sursa setului de date:\n";
    cout<<"1 - din fisier (data.txt)\n";
    cout<<"2 - generat de program\n";
    cout<<"3 - Calculeaza mediile din toate fisierele CSV\n";
    cout<<"Alegeti sursa setului de date: \n";
    while(!(cin>>sursa) || sursa<1 || sursa>3)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"Date invalide. Incercati din nou:\n";
    }
    if(sursa==3)
    {
        computeAverages();
        cout<<"Apasati Enter pentru a inchide programul..."<<endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return 0;
    }
    if(sursa==1)
    {
        ifstream fin("data.txt");
        int n;
        if(!(fin>>n) || n<=0 || n>10000000)
        {
            cout<<"Fisier invalid. Programul se va inchide.\n";
            return 0;
        }
        v.resize(n);
        for(int i=0;i<n;i++)
        {
            if(!(fin>>v[i]))
            {
                cout<<"Fisier invalid. Programul se va inchide.\n";
                return 0;
            }
        }
        fin.close();
        datasetName="data-set-results";
    }
    else if(sursa==2)
    {
        cout<<"Tipul listei:\n";
        cout<<"1 - aleatoare\n";
        cout<<"2 - sortata crescator\n";
        cout<<"3 - sortata descrescator\n";
        cout<<"4 - aproape sortata\n";
        cout<<"5 - valori putine distincte\n";
        cout<<"Alegeti tipul listei: \n";
        while(!(cin>>tip) || tip<1 || tip>5)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Date invalide. Incercati din nou:\n";
        }
        cout<<"Introduceti dimensiunea setului de date: \n";
        while(!(cin>>s) || s<=0 || s>10000000)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Date invalide. Incercati din nou:\n";
        }
        if(tip==1)
        {
            datasetName="random";
            generateRandomArray(v,s);
        }
        else if(tip==2)
        {
            datasetName="sorted";
            generateSortedArray(v,s);
        }
        else if(tip==3)
        {
            datasetName="reverse";
            generateReverseArray(v,s);
        }
        else if(tip==4)
        {
            datasetName="almost_sorted";
            generateAlmostSortedArray(v,s);
        }
        else if(tip==5)
        {
            datasetName="few_unique";
            generateFewUniqueArray(v,s);
        }
    }
    system("cls");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    for(int i=0;i<47;i++)
    {
        cout<<"=";
    }
    cout<<endl<<"     ";
    cout<<"| Eficienta algoritmilor de sortare |"<<endl;
    for(int i=0;i<47;i++)
    {
        cout<<"=";
    }
    cout<<endl<<endl;
    cout<<"Apasati Enter pentru a incepe testarea algoritmilor de sortare..."<<endl;
    cin.ignore();
    cin.get();
    system("cls");
    string runTimestamp = getFilenameTimestamp();
    ofstream genfout("generated_list_"+datasetName+"_n"+to_string(v.size())+"_"+runTimestamp+".txt");
    for(int i=0;i<v.size();i++)
    {
        genfout<<v[i]<<"\n";
    }
    genfout.close();
    vector<int> v1=v,v2=v,v3=v,v4=v,v5=v,v6=v;
    results.clear();
    thread t1(monitor,"MergeSort",bind(mergeSort,ref(v4),0,v4.size()-1));
    thread t2(monitor,"QuickSort",bind(quickSort,ref(v5),0,v5.size()-1));
    thread t3(monitor,"HeapSort",bind(heapSort,ref(v6)));
    thread t4(monitor,"BubbleSort",bind(bubbleSort,ref(v1)));
    thread t5(monitor,"SelectionSort",bind(selectionSort,ref(v2)));
    thread t6(monitor,"InsertionSort",bind(insertionSort,ref(v3)));
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    sort(results.begin(), results.end(), [](const Result& a, const Result& b)
    {
      return a.time_ns<b.time_ns;
    });
    if (!results.empty()) 
    {
         cout<<"\nCel mai rapid algoritm: "<<results[0].name<<" ("<<formatPrecise(results[0].time_ns)<<")\n";
    }
        string filename="results_"+datasetName+"_n"+to_string(v.size())+"_"+runTimestamp+".csv";
        ofstream fout(filename);
        fout<<"Algoritm,Timp_ns,Timp_ms,Timp_formatat,RAM_bytes,RAM_KB,RAM_MB\n";
    for(const auto& res : results)
    {
         fout<<res.name<< ","
         <<res.time_ns<<","
         <<res.time_ms<<","
            <<formatPrecise(res.time_ns)<<","
            <<res.memory_bytes<<","
            <<res.memory_bytes/1024<<","
            <<res.memory_bytes/(1024*1024)
            <<"\n";
    }
    fout.close();
        cout<<"\nTipul setului de date: "<<datasetName<<"\n";
        cout<<"Rezultatele au fost salvate in "<<filename<<"\n";
        cout<<"Apasati Enter pentru a inchide programul..."<<endl;
    cin.ignore();
    cin.get();
    return 0;
} 