
//classe da estrutura de dados (recurso) compartilhado entre as threads
class SomaTotal {
    //recurso compartilhado entre as threads
    private int somaTotal;

    //construtor
    public SomaTotal(){
        this.somaTotal = 0;
    }

    public synchronized void inc(){
        this.somaTotal++;
    }

    public synchronized int getSomaTotal(){
        return this.somaTotal;
    }
}

//classe que estende Thread e implementa a tarefa de cada thread do programa
class T extends Thread {
     //identificador da thread
     private int id;

     //objeto compartilhado entre as threads
     SomaTotal soma;

     private int vetor[];
     private int nthreads;


     //construtor
    public T(int tid, int nthreads,int vetor[], SomaTotal soma){
        this.id = tid;
        this.nthreads = nthreads;
        this.vetor = vetor;
        this.soma = soma;
    }

    //metodo main da thread
    public void run(){
        System.out.println("Thread " + this.id + " iniciou!");
        for(int i = this.id; i<vetor.length;i+=this.nthreads){
            if (this.vetor[i] % 2 == 0){
                soma.inc();
            }
        }
        System.out.println("Thread " + this.id + " finalizou!");

    }
}


public class ParThread {
    static final int N = 3; //nthreads
    private static int nelementos = 100; //numero de elementos no vetor
    private static int[] vetor = new int[nelementos];

    public static void main(String[] args) {
        //reserva espaço para um vetor de threads
        Thread[] threads  = new Thread[N];

        //preenchendo o vetor
        for(int i = 0; i<nelementos; i++){
            vetor[i] = i;
        }

        //printa vetor
        //for(int i=0; i < vetor.length ; i++){
        //    System.out.println(vetor[i]);
        //}

        //cria uma instancia do recurso compartilhado entre as threads
        SomaTotal soma = new SomaTotal();

        //cria threads de aplicaçao
        for(int i = 0; i<threads.length;i++){
            threads[i] = new T(i,N,vetor,soma);
        }

        //inicia as threads
        for(int i = 0;i<threads.length;i++){
            threads[i].start();
        }

        //espera pelo termino de todas as threads
        for(int i=0; i<threads.length;i++){
            try { threads[i].join(); } catch (InterruptedException e) { return; }
        }

        System.out.println(soma.getSomaTotal());
    }

}
