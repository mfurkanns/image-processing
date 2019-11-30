import cv2
from PIL import Image
import os, os.path
import glob


class hist:   # hist classı b,g,r ve lbp histogramlarını tutan member fieldlara sahiptir.
    def __init__(self,image):
        self.img = image        
        self.b=[]
        self.g=[]
        self.r=[]
        self.lbp=[]
        self.img0 = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
        
    def createHistogram(self):  # bu metodla b,g,r ve lbp histogramları oluşturulur.
        for i in range(256): # dizilerin içi 0'larla doldurulur.
            self.b.append(0)
            self.g.append(0)
            self.r.append(0)
            self.lbp.append(0)
        for i in range(1,self.img.shape[0]-1): # fotoğrafı incelemeye [1,1]den başlanır
            for j in range(1,self.img.shape[1]-1):
                self.b[self.img[i][j][0]]+=1 # blue histogramı olşuturulur.
                self.g[self.img[i][j][1]]+=1 # green histogramı olşuturulur.
                self.r[self.img[i][j][2]]+=1 # red histogramı olşuturulur.
                tmp=0  # local binary partition histogramı oluşturulur.
                if(self.img0[i-1][j-1]>self.img0[i][j]): # sol üst komşuya bakılır.
                    tmp+=128
                if(self.img0[i-1][j]>self.img0[i][j]): # üst komşuya bakılır.
                    tmp+=64
                if(self.img0[i-1][j+1]>self.img0[i][j]): # sağ üst komşuya bakılır.
                    tmp+=32
                if(self.img0[i][j-1]>self.img0[i][j]): # sol komşuya bakılır.
                    tmp+=16
                if(self.img0[i][j+1]>self.img0[i][j]): # sağ komşuya bakılır.
                    tmp+=8
                if(self.img0[i+1][j-1]>self.img0[i][j]): # sol alt komşuya bakılır.
                    tmp+=4
                if(self.img0[i+1][j]>self.img0[i][j]): # alt komşuya bakılır.
                    tmp+=2
                if(self.img0[i+1][j+1]>self.img0[i][j]): # sağ alt komşuya bakılır.
                    tmp+=1                    
                self.lbp[tmp]+=1
            
            
        for i in range(256): # histogramlar [0,1] aralığına normalize edilir.
            self.b[i] = self.b[i] / (self.img.shape[0]*self.img.shape[1]) 
            self.g[i] = self.g[i] / (self.img.shape[0]*self.img.shape[1])
            self.r[i] = self.r[i] / (self.img.shape[0]*self.img.shape[1])
            self.lbp[i] = self.lbp[i] / (self.img0.shape[0]*self.img0.shape[1])


def readImgFromDirectory(path): # klasördeki fotoğraflar diziye alınır
    image_list = []
    for filename in glob.glob(path):        
        im = cv2.imread(filename,1) 
        image_list.append(im)
                  
    return image_list


def rgbDistance(h1,h2): # rgb için Manhattan distance hesaplanır. 
    dist = 0
    for i in range(256):        
        dist += abs(h1.b[i]-h2.b[i])+abs(h1.g[i]-h2.g[i])+abs(h1.r[i]-h2.r[i])
    return dist

def lbpDistance(h1,h2): # lbp için Manhattan distance hesaplanır.
    dist = 0
    for i in range(256):        
        dist += abs(h1.lbp[i]-h2.lbp[i])

    return dist
        


def histograms(imageList): # fotoğların histogramları elde edilir.
    
    histogram_list = []
    for i in imageList:
        tmpHist = hist(i)
        tmpHist.createHistogram()  
        histogram_list.append(tmpHist)
        
    return histogram_list  # histogram listesi elde edilir.




def findSimilar(database_hist,input_hist,databaseImages,inputImages): # en yakın 5 fotoğraf bulunur.
    
    for i in range(0,len(inputImages)):        
        fotoInd = i
     #   cv2.imshow(str(fotoInd)+'. original foto',inputImages[fotoInd]) # test edilen fotoğraf gösterilir
        dist_list = [] # her fotoğrafa olan uzaklığı tutan dizidir.
        indx_list = [] # fotoğrafların indislerini tutan dizidir.
        for j in range(0,len(database_hist)): # bütün fotoğraflar için
            tmp = lbpDistance(database_hist[j],input_hist[fotoInd]) # sadece lbp icin distance bulunur
            dist_list.append(tmp) # uzaklık diziye eklenir.
            indx_list.append(j) # fotoğrafın indisi diziye eklenir.
        
        for k in range(len(dist_list)): # uzaklıklar küçükten büyüğe sıralanır.
            for s in range(0,70-k-1):
                if dist_list[s] > dist_list[s + 1]:
                    dist_list[s], dist_list[s + 1] = dist_list[s + 1], dist_list[s]
                    indx_list[s], indx_list[s + 1] = indx_list[s + 1], indx_list[s] # sıralandıkça index listesi güncellenir.
                
        for i in range(5):
        #    cv2.imshow(str(i+1)+"lbp",databaseImages[indx_list[i]])
            print(fotoInd,'.fotoya lbp olarak en benzer = ',indx_list[i])  # benzeyenler yazdırılır.  
    
    
        # aynı işlemler bu sefer sadece rgb için yapılır
        
        rgb_dist_list = []
        rgb_indx_list = []
        tmp = 0
        
        for j in range(0,len(database_hist)):
            tmp = rgbDistance(database_hist[j],input_hist[fotoInd])
            rgb_dist_list.append(tmp)
            rgb_indx_list.append(j)
            
        for k in range(len(rgb_dist_list)):
            for s in range(0,70-k-1):
                if rgb_dist_list[s] > rgb_dist_list[s + 1]:
                    rgb_dist_list[s], rgb_dist_list[s + 1] = rgb_dist_list[s + 1], rgb_dist_list[s]
                    rgb_indx_list[s], rgb_indx_list[s + 1] = rgb_indx_list[s + 1], rgb_indx_list[s]
        
        for i in range(5):
         #   cv2.imshow(str(i+1)+"rgb",databaseImages[rgb_indx_list[i]])
            print(fotoInd,'.fotoya rgb olarak en benzer = ',rgb_indx_list[i])
    
        #    aynı işlemler rgb + lbp için yapılır.
        
        dist_list = []
        indx_list = []
        tmp = 0        
        mindist = rgbDistance(database_hist[0],input_hist[fotoInd]) + lbpDistance(database_hist[j],input_hist[fotoInd])
        dist_list.append(mindist)
        indx_list.append(0)
        
        for j in range(1,len(database_hist)):
            tmp = rgbDistance(database_hist[j],input_hist[fotoInd])+lbpDistance(database_hist[j],input_hist[fotoInd])
            dist_list.append(tmp)
            indx_list.append(j)
            
        for k in range(len(dist_list)):
            for s in range(0,70-k-1):
                if dist_list[s] > dist_list[s + 1]:
                    dist_list[s], dist_list[s + 1] = dist_list[s + 1], dist_list[s]
                    indx_list[s], indx_list[s + 1] = indx_list[s + 1], indx_list[s]
        
        for i in range(5):
        #    cv2.imshow(str(i+1)+"rbg+lbp",databaseImages[indx_list[i]])
            print(fotoInd,'.fotoya rgb+lbp olarak en benzer = ',indx_list[i])
        
        
           
databaseImages = readImgFromDirectory('databaseImages/*') 
database_hist = histograms(databaseImages)

inputImages = readImgFromDirectory('inputImages/*')
input_hist = histograms(inputImages)


findSimilar(database_hist,input_hist,databaseImages,inputImages)


