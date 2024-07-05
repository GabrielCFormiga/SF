#roda todas as entradas 20 vezes
import os
import sys
from subprocess import call

#extrai os dados da entrada a partir do nome do arquivo
def getInputInfo(line):
   
   #pega o BC
 #  index_ini=line.index("BC")+2
  # index_fim=line.index("/",index_ini)
  # bc=int(line[index_ini:index_fim])
   
   #pega o V
   index_ini=line.index("V")+2
   index_fim=line.index("_",index_ini)
   v=int(line[index_ini:index_fim])

   #pega o U
   index_ini=line.index("U")+2
   index_fim=line.index("_",index_ini)
   u=int(line[index_ini:index_fim])

   #pega o D
   index_ini=line.index("D")+2
   index_fim=line.index("_",index_ini)
   d=float(line[index_ini:index_fim])
   
   return (-1,v,u,d)

def main():
#for al in range(0,1):
	 for bc in range(2,5):
   		dir="entrada/BC"+str(bc)
#grasp="grasp_solucoes/BC"+str(bc)
   		inputs=os.listdir(dir)
		#executa cada uma das entradas
  		for input in inputs:
   			print "running "+input
#call("./bc "+dir+"/"+input + " " +grasp+"/"+input + " " + str(al),shell=True)
   			call("./bc "+dir+"/"+input, shell=True)


if __name__=="__main__":
   main()

