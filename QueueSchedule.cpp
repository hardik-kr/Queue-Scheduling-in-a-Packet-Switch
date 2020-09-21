#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_map>
#include<cmath>
#include<cstring>
#include<list>
#include<time.h>
#include<map>
#include<random>
using namespace std ;

//Input variables

//No. of input, output ports
int N,O ;      
//Buffer size 
int B ;         
//packet generation probability
float p ;         
//scheduling algo
string queue ;  
//Knockout   
int K ;         
//output_file
string outfile ;
//maxtimeslot   
int T ;         

//Data Structure

//Input Buffer at each switch port
vector<list<int> > inpBuffer ;
//Output Buffer at each switch port
vector<list<int> > outBuffer ;
//structure to store information about the packet
struct PacketInformation
{
    int InputPort ;
    int OutputPort ;
    double arrivalTime ;
    double deptTime ;
    bool linkcheck ;
};
vector<PacketInformation> packetInfo ;

//randomly generate arrival time, output port
vector<pair<double,int> > input ;

//iSLIP request Phase 
map<int,vector<int>> Request ;

//Generator to find random nos
default_random_engine generator ;

//Kouq drop probability
double KOUQdropprob ;

void TrafficGeneration(int timeSlot)
{
    input.clear();
    input.resize(N) ;

    //initialize the 
    for(int i=0 ; i<N ; i++)
    {
        input[i].first=-1 ;
        input[i].second=-1 ;
    }
    double temp = (double)(N)/p ;
    //srand(time(0));
    uniform_int_distribution<int> outprt(0,N-1) ;
    uniform_real_distribution<double> arrtime(timeSlot+0.001,timeSlot+0.01) ;

    for(int i=0 ; i<N ;i++)
    {
        //srand(time(0));
        int val = rand()%(int)(ceil(temp)) ;
        if(val<N)
        {
            input[i].first = arrtime(generator) ;
            input[i].second = outprt(generator) ;
        }
    }


}

void INQ()
{
    for(int i=0 ; i<N ; i++)
    {
        if(input[i].first!=-1)
        {
            if(inpBuffer[i].size()<B)  
            {
                struct PacketInformation pi ;
                pi.InputPort = i ;  // Input_Port
                pi.OutputPort = input[i].second   ;//Output_port
                pi.arrivalTime = input[i].first ;   //arrival_time
                pi.deptTime = -1 ;
                packetInfo.push_back(pi);
                inpBuffer[i].push_back(packetInfo.size()-1) ;
            }
        }
    }

    //It is used to store the total input ports corresponds to the particular output port
    //<outport, array of input_port>
    unordered_map<int,vector<int>> count ;

    //enter packets into the input buffer at each input port
    for(int i=0 ; i<N ; i++)
    {
        if(!inpBuffer[i].empty())
        {
            int packidx = inpBuffer[i].front() ;
            int countidx =  packetInfo[packidx].OutputPort ;
            count[countidx].push_back(i) ;
        }
    }

    //int b = 10 ;
    //enter packets into the input buffer at each input port
    int j=0 ;
    for(int i=0 ; i<N ; i++)
    {
        if(!inpBuffer[i].empty())
        {
            int packidx = inpBuffer[i].front() ;
            int outidx =  packetInfo[packidx].OutputPort ;            
            if(count[outidx].size() == 1)
            {
                if(outBuffer[outidx].size()<B)
                {
                    outBuffer[outidx].push_back(packidx) ;

                    inpBuffer[i].pop_front() ;
                }
            }
            else if(count[outidx].size() >=2 && j==0)
            { 
                if(outBuffer[outidx].size()<B)
                {
                    int len = count[outidx].size() ;
                    uniform_int_distribution<int> idx(0,len-1) ;
                    int randprt = idx(generator) ;
                    outBuffer[outidx].push_back(inpBuffer[count[outidx][randprt]].front()) ;
                    inpBuffer[count[outidx][randprt]].pop_front();
                    j=1 ;
                }
            }
        }
    }
    
}

void KOUQ()
{
    double prob =0;
    for(int i=0 ; i<N ; i++)
    {
        if(input[i].first!=-1)
        {
            if(inpBuffer[i].size()<B)  
            {
                struct PacketInformation pi ;
                pi.InputPort = i ;  // Input_Port
                pi.OutputPort = input[i].second   ;//Output_port
                pi.arrivalTime = input[i].first ;   //arrival_time
                pi.deptTime = -1 ;
                packetInfo.push_back(pi);
                inpBuffer[i].push_back(packetInfo.size()-1) ;
            }
        }
    }
    bool flag = 0 ;

    unordered_map<int,vector<int>> count ;
    //enter packets into the input buffer at each input port
    for(int i=0 ; i<N ; i++)
    {
        if(!inpBuffer[i].empty())
        {
            int outprt = packetInfo[inpBuffer[i].front()].OutputPort ;
            count[outprt].push_back(-1) ;
            int k = count[outprt].size() -2 ;
            while(k>=0)
            {
                double inparrtime = packetInfo[inpBuffer[i].front()].arrivalTime ;
                double countarrtime = packetInfo[inpBuffer[count[outprt][k]].front()].arrivalTime ; 
                if(inparrtime <= countarrtime)
                {
                    count[outprt][k+1] = count[outprt][k] ;
                }
                else
                break ;
                k-- ;
            }
            count[outprt][k+1] = i ;
        }
    }

    for(int i=0 ; i<N ; i++)
    {
        flag = 0;
        if(count[i].size() == 1)
        {
            if(outBuffer[i].size() < K)
            {
                outBuffer[i].push_back(inpBuffer[count[i][0]].front()) ;
                inpBuffer[count[i][0]].pop_front() ;
            }
        }
        else if(count[i].size() >= 2)
        {
            int totinpport = count[i].size() ;
            int j=outBuffer[i].size() ;
            int k= 0;
            while(j< K && k<totinpport) //K is output size Buffer
            {
                int ct = count[i][k] ;
                outBuffer[i].push_back(inpBuffer[ct].front()) ;
                inpBuffer[ct].pop_front() ;
                j++ ;
                k++ ;
            }
            while(k<totinpport)
            {
                inpBuffer[count[i][k]].pop_front() ;
                k++ ;
                if(!flag)
                prob++ ;
                flag=1 ;
            }
        }
    }
    KOUQdropprob+= prob/(double)N ;


}

void iSLIP()
{
    //Count for total output port for corresponding input i.e index
    int countoutPort[N] ;
    for(int i= 0  ;i<N  ; i++) countoutPort[i]=-1 ;
    
    bool checkPorts[N] ;
    for(int i= 0  ;i<N  ; i++) checkPorts[i]=0 ;

    vector<int>::iterator index ;

    for(int i=0 ; i<N ; i++)
    {
        if(input[i].first!=-1)
        {
            if(Request[i].size()<B)
            {
                struct PacketInformation pi ;
                pi.InputPort = i ;  // Input_Port
                pi.OutputPort = input[i].second   ;//Output_port
                pi.arrivalTime = input[i].first ;   //arrival_time
                pi.deptTime = -1 ;
                packetInfo.push_back(pi);
                Request[i].push_back(packetInfo.size()-1) ;
            }
        }
        
        if(!Request[i].empty())
        {
            index = Request[i].begin() ;
            for(int j=0 ; j<Request[i].size() ; j++)
            {
                int inpthport = Request[i][j] ;
                int outport = packetInfo[inpthport].OutputPort ;
                if(checkPorts[outport]==0)
                {
                    Request[i].erase(index+j) ;
                    countoutPort[i] = inpthport ;
                    checkPorts[outport] = 1;
                    break ;
                }
            }
        }
        if(countoutPort[i] !=-1)
        {
            if(outBuffer[i].size()<B)
            {
                int outprt = packetInfo[countoutPort[i]].OutputPort ;
                outBuffer[outprt].push_back(countoutPort[i]) ;
            }
        }
    }
}

void PacketSchedule()
{
    if(queue=="INQ")
    INQ() ;
    else if(queue=="KOUQ")
    KOUQ() ;
    else
    {
        iSLIP() ;
    }
}

void PacketTransmission(int timeSlot) 
{
    for(int i=0 ; i<N ; i++)
    {
        if(!outBuffer[i].empty())
        {
            int packidx = outBuffer[i].front() ;
            outBuffer[i].pop_front() ;
            packetInfo[packidx].deptTime = timeSlot ;
            packetInfo[packidx].linkcheck = 0 ;
        }
    }

}
double LinkUtilization()
{
    //hash map to count total no. of links in entire simulation duration
    vector<vector<int>> ActiveLinkCount(N) ;

    for(int i=0 ; i<N ; i++)
    ActiveLinkCount[i] = vector<int>(N) ;

    for(int i=0 ; i<packetInfo.size() ; i++)
    {
        if(packetInfo[i].deptTime!=-1)
        ActiveLinkCount[packetInfo[i].InputPort][packetInfo[i].OutputPort]++ ;
    }

    double sum=0 ;
    for(int i=0 ; i<N ; i++)
    {
        for(int j=0 ; j<N ; j++)
        {
            sum += ActiveLinkCount[i][j] ;
        }
    }
    //cout<<"\n\nTransmit pack : "<<sum<<endl<<endl; 

    return sum/(double)N/(double)T ;

}


double AvgDelay()
{
    int len=0 ;
    double sum=0 ;
    bool f=0 ;
    for(int i=0 ; i<packetInfo.size() ; i++)
    {
        if(packetInfo[i].deptTime!=-1)
        {
            f=1 ;
            len++ ;
            sum+=(packetInfo[i].deptTime - floor(packetInfo[i].arrivalTime)) ;
        }
    }
    return sum/len ;

}

double StdDevPD()
{
    int len=0 ;
    double sum=0 ;
    bool f=0 ;
    vector<double> stddev ;
    for(int i=0 ; i<packetInfo.size() ; i++)
    {
        if(packetInfo[i].deptTime!=-1)
        {
            f=1 ;
            len++ ;
            sum+=(packetInfo[i].deptTime - packetInfo[i].arrivalTime) ;
            stddev.push_back(packetInfo[i].deptTime - packetInfo[i].arrivalTime) ;
        }
    }
    double mean = sum/len ;
    sum = 0 ;
    for(int i=0 ; i<stddev.size() ; i++)
    {
        sum+=pow(stddev[i]-mean,2) ;
    }
    return sqrt(sum/len) ;

}

void FinalOutput()
{
    cout<<"\t-------------------------------------------------------------------------------------------------------------------"<<endl ;
    cout<<"\t N\t\tp\t\tQueue\t\tAvg PD\t\tStd Dev of PD\t\tAvg Link Utilization"<<endl<<endl  ;
    cout<<"\t------------------------------------------------------------------------------------------------------------------- "<<endl ;

    cout<<"\t "<<N<<"\t\t"<<p<<"\t\t"<<queue<<"\t\t" ;
    cout<<AvgDelay()<<"\t\t" ;
    cout<<StdDevPD()<<"\t\t";
    cout<<LinkUtilization()<<endl<<endl<<endl<<endl ;

    if(queue=="KOUQ")
    cout<<"KOUQ Drop Probability : "<<KOUQdropprob/T<<endl<<endl ;

    cout<<"Output file is also created with file name : \""<<outfile<<"\""<<endl<<endl ;

    ofstream fout ;
    fout.open(outfile) ;


    fout<<"\t-------------------------------------------------------------------------------------------------------------------"<<endl ;
    fout<<"\t N\t\tp\t\tQueue\t\tAvg PD\t\tStd Dev of PD\t\tAvg Link Utilization"<<endl<<endl  ;
    fout<<"\t------------------------------------------------------------------------------------------------------------------- "<<endl ;

    fout<<"\t "<<N<<"\t\t"<<p<<"\t\t"<<queue<<"\t\t" ;
    fout<<AvgDelay()<<"\t\t" ;
    fout<<StdDevPD()<<"\t\t";
    fout<<LinkUtilization()<<endl<<endl ;
    if(queue=="KOUQ")
    fout<<"KOUQ Drop Probability : "<<KOUQdropprob/T<<endl<<endl ;
    fout.close() ;

}

int main(int argc,char **argv)
{

    if(argc<=7)
    {
        cout<<"\n\nSome Input parameter are missing "<<endl ;
        cout<<"Enter all the required 7 input parameter"<<endl<<endl ;
        exit(0) ;
    }

    //input value from command line
    N = stoi(argv[1]) ;
    B = stoi(argv[2]) ;
    p = stof(argv[3]) ;
    queue = argv[4] ;
    K = stoi(argv[5]) ;
    outfile = argv[6] ;
    T = stoi(argv[7]) ;

    inpBuffer.resize(N) ;
    outBuffer.resize(N) ;

    if(queue!="INQ" && queue!="KOUQ" && queue!="ISLIP")
    {
        cout<<"INVALID scheduling algorithm"<<endl ;
        exit(0);
    }

    for(int i=0 ; i<T ; i++)
    {
        if(i==0)
        TrafficGeneration(i);
        else if(i==1)
        {

            PacketSchedule();
            TrafficGeneration(i) ;
        }
        else
        {
            PacketTransmission(i);
            PacketSchedule();
            TrafficGeneration(i);
        }
        
    }
    FinalOutput();

    return 0 ;
}