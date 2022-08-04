import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import numpy as np
import sys
sns.set_style("white")




class PlotDistrib:
    
    def __init__(self, fpath, df, frac=0.0001, title=None, fig_height=12, width_ratio=5):
        self.frac=frac
        self.title=title
        self.fig_height = fig_height
        self.width_ratio = width_ratio
        self.fpath = fpath
        self.subsample(df)
        self.create_graph()
        self.save_graph()
         
    def subsample(self,df):
        self.data = df.groupby(by='library')\
            .sample(frac=self.frac)\
                .reset_index(drop=True)
                
    def create_graph(self):
        self.graph = sns.JointGrid(height=self.fig_height,ratio=self.width_ratio)
        sns.histplot(data=self.data, 
                    x='read_length', 
                    y='read_avg_quality',
                    ax=self.graph.ax_joint,
                    log_scale=(True, False))
        sns.boxplot(data=self.data, x='read_length',ax=self.graph.ax_marg_x, color='white')
        sns.boxplot(data=self.data, y='read_avg_quality',ax=self.graph.ax_marg_y, color='white')
        self.graph.refline(x=self.data.read_length.median(), 
                y=self.data.read_avg_quality.median())
        self.graph.ax_joint.set_xlabel("length (bp) [log-scale]", fontsize=15, y=-0.2)
        self.graph.ax_joint.set_ylabel("quality (phred)", fontsize=15, x=-0.2)
        if self.title:    
            plt.suptitle(str(self.title), fontsize=20, y=1.05)
            
    def save_graph(self):
        plt.savefig(self.fpath, bbox_inches='tight')
        
        
        
class PlotCoverageTradeoff:
    """
    Generate a plot (heatmap) representing the genome coverage for different
    reads' length and quality. 
    trade-offs. 
    
    Attributes :
    ------------
    fpath (str)             --  output plot file path.
    df (pandas dataframe)   --  dataframe containing the following columns : 
                                'read_length' and 'read_avg_quality'.
    title (str)             --  figure title (default : None).
    fig_size (tuple)        --  figure width and height 
    
    Methods :
    ---------
    create_graph()          --  Create a heatmap from coverage values for 
                                different quality and length cut-off. 
                                Min. quality is represented on the y axis and 
                                length on the x axis.
    save_grapth()           --  Save the generated plot as a file.
    """
    
    def __init__(self, fpath, df, title=None, fig_size=(12,10)):
        self.data = df
        self.fpath = fpath
        self.title = title
        self.fig_size = fig_size

        
        
    def create_graph(self):
        """
        Create a heatmap from coverage values at different quality and length 
        cut-off. Min. quality is represented on the y axis and length on the x axis.
        """
        plt.figure(figsize=self.fig_size)
        ax1 = sns.heatmap(
            self.data[['min_quality', 'min_length', 'coverage']].round(1)\
                .pivot(index='min_quality', columns='min_length', values='coverage'),
                    cbar_kws = dict(location="bottom"))
        ax1.figure.axes[-1].set_xlabel('Coverage (X)', size=15)

        if self.title:
            plt.title(self.title, fontsize=20)
        plt.xlabel("min. length (bp)", fontsize=15)
        plt.ylabel("min. quality (phred)", fontsize=15)
        plt.xticks(rotation=90);
            
    def save_graph(self):
        """
        Save the generated plot as a file
        """
        plt.savefig(self.fpath)


        
        

