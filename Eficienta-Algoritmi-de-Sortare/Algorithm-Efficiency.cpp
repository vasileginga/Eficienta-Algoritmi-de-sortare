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
using namespace std;
void bubbleSort(vector<int>&v);
void selectionSort(vector<int>&v);
void insertionSort(vector<int>&v);
void merge(vector<int>&v,int l,int m,int r);
void mergeSort(vector<int>&v,int l,int r);
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
struct Result {
    string name;
    long long ms;
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
void monitor(string name, function<void()> func) 
{
    atomic<bool> ver(false);
    thread monitorThread([&](){
        while(!ver) 
        {
            {
            lock_guard<mutex> lock(mtx);
            cout<<"[Running] "<<name<<" at ["<<getCurrentTime()<<"]\n";
            }
            this_thread::sleep_for(chrono::seconds(5));
        }
    });
    {
        lock_guard<mutex> lock(mtx);
        cout<<"[Started] "<<name<<" at ["<<getCurrentTime()<<"]\n";
    }
    auto start=chrono::high_resolution_clock::now();
    func();
    ver=true;
    monitorThread.join();
    auto end=chrono::high_resolution_clock::now();
    auto duration=chrono::duration_cast<chrono::milliseconds>(end - start);
    {
        lock_guard<mutex> lock(mtx);
        results.push_back({name, duration.count()});
        cout<<"[Finished] "<<name<<" at ["<<getCurrentTime()<<"] took "<<formatDuration(duration.count())<<"\n";
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
void quickSort(vector<int>&v,int low,int high) 
{
    if(low<high) 
    {
        int pivot=v[high];
        int i=low-1;
        for(int j=low;j<high;j++) 
        {
            if(v[j]<pivot) 
            {
                i++;
                swap(v[i],v[j]);
            }
        }
        swap(v[i+1],v[high]);
        int pi=i+1;
        quickSort(v,low,pi-1);
        quickSort(v,pi+1,high);
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
    cout<<"Alegeti sursa setului de date: \n";
    while(!(cin>>sursa) || sursa<1 || sursa>2)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout<<"Input invalid. Incercati din nou:\n";
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
        datasetName="file";
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
            cout<<"Input invalid. Incercati din nou:\n";
        }
        cout<<"Introduceti dimensiunea setului de date: \n";
        while(!(cin>>s) || s<=0 || s>10000000)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"Input invalid. Incercati din nou:\n";
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
      return a.ms<b.ms;
    });
    if (!results.empty()) 
    {
         cout<<"\nFastest algorithm: "<<results[0].name<<" ("<<formatDuration(results[0].ms)<<")\n";
    }
        string filename="results_"+datasetName+".csv";
        ofstream fout(filename);
    fout<<"Algoritm,Timp(ms),Durata\n";
    for(const auto& res : results)
    {
         fout<<res.name<< ","
         <<res.ms<<","
         <<formatDuration(res.ms)<<"\n";
    }
    fout.close();
        cout<<"\nDataset type: "<<datasetName<<"\n";
        cout<<"Results saved in "<<filename<<"\n";
        cout<<"Apasați Enter pentru a inchide programul..."<<endl;
    cin.ignore();
    cin.get();
    return 0;
} 