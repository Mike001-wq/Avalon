#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
using namespace std;

int adj[1001][1001];
bool visited[1001];
int each_min[1001];

int main()
{
    int t,n;
    cin>>t>>n;
    memset(visited,0,1001);
     for(int i=1;i<=n;i++)for(int j=1;j<=n;j++)adj[i][j]=1000;
    for(int i=1;i<=t;i++){
        int from,to,dis;
        cin>>from>>to>>dis;
        adj[from][to]=dis;
        adj[to][from]=dis;
    }
    for(int i=1;i<=n;i++)each_min[i]=1000;
    for(int i=1;i<=n;i++){
    if(adj[n][i]<each_min[i])each_min[i]=adj[n][i];
    }
    visited[n]=true;
    while(!visited[1]){
        int mi=1000;
        int mi_index;
        for(int j=1;j<=n;j++)if((!visited[j])&&(each_min[j]<mi)){mi=each_min[j];mi_index=j;}
        visited[mi_index]=true;
        for(int i=1;i<=n;i++){
        if((!visited[i])&&(adj[mi_index][i]+each_min[mi_index]<each_min[i]))each_min[i]=adj[mi_index][i]+each_min[mi_index];
        }
    }
    cout<<each_min[1]<<endl;
    return 0;
}
