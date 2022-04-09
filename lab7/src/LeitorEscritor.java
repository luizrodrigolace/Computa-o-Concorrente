class variavelGlobal{
    public static int variavel = 0;
}

// Monitor que implementa a logica do padrao leitores/escritores
class Monitor {
    private int leit, escr;

    // Construtor
    Monitor() {
        this.leit = 0; //leitores lendo (0 ou mais)
        this.escr = 0; //escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            while (this.escr > 0) {
                System.out.println ("le.leitorBloqueado("+id+")" + " valor da variavel global:" + variavelGlobal.variavel);
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.leit++;  //registra que ha mais um leitor lendo
            System.out.println ("le.leitorLendo("+id+")" + " valor da variavel global:" + variavelGlobal.variavel);
        } catch (InterruptedException e) { }
    }

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); //libera escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorSaindo("+id+")" + " valor da variavel global:" + variavelGlobal.variavel);
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
                System.out.println ("le.escritorBloqueado("+id+")" + " valor da variavel global:" + variavelGlobal.variavel);
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++; //registra que ha um escritor escrevendo
            System.out.println ("le.escritorEscrevendo("+id+")"+ " valor da variavel global:" + variavelGlobal.variavel);
        } catch (InterruptedException e) { }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println ("le.escritorSaindo("+id+")" + " valor da variavel global:" + variavelGlobal.variavel);
    }
}



//Aplicacao de exemplo--------------------------------------------------------
// Leitor
class Leitor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo
    Monitor monitor;//objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Leitor (int id, int delayTime, Monitor m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        int multiplos = 0;

        try {
            for (int j = 0; j<principal.interacoes; j++) {
                this.monitor.EntraLeitor(this.id);

                for (int i=1; i<=variavelGlobal.variavel; i++){
                    if(variavelGlobal.variavel % i == 0){
                        multiplos++;
                    }
                }

                if (multiplos == 2){
                    System.out.println(" O valor da variavel global:" + variavelGlobal.variavel + " é primo!");
                }
                else{
                    System.out.println(" O valor da variavel global:" + variavelGlobal.variavel + " não é primo!");
                }

                multiplos = 0;
                this.monitor.SaiLeitor(this.id);
                sleep(this.delay);
            }
        } catch (InterruptedException e) { return; }
    }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
    int id; //identificador da thread
    int delay; //atraso bobo...
    Monitor monitor; //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    Escritor (int id, int delayTime, Monitor m) {
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        double j=777777777.7, i;
        try {
            for (int k= 0;k<principal.interacoes;k++) {
                this.monitor.EntraEscritor(this.id);
                variavelGlobal.variavel = id;
                this.monitor.SaiEscritor(this.id);
                sleep(this.delay); //atraso bobo...
            }
        } catch (InterruptedException e) { return; }
    }
}

class LeitorEscritor extends Thread{
    int id;
    int delay;
    Monitor monitor;

    LeitorEscritor(int id, int delayTime, Monitor m){
        this.id = id;
        this.delay = delayTime;
        this.monitor = m;
    }

    public void run(){
        try{
            for(int i = 0; i <principal.interacoes;i++){
                this.monitor.EntraLeitor(this.id);
                if (variavelGlobal.variavel % 2 == 0){
                    System.out.println(" o valor atual da variavel global: " + variavelGlobal.variavel + " é um valor par");
                }
                else{
                    System.out.println(" o valor atual da variavel global: " + variavelGlobal.variavel + " é um valor impar");
                }
                this.monitor.SaiLeitor(this.id);
                sleep(this.delay);

                this.monitor.EntraEscritor(this.id);
                System.out.println("Dobrando o valor da variavel global: de "+ variavelGlobal.variavel + " para " + variavelGlobal.variavel * 2);
                variavelGlobal.variavel *= 2;
                this.monitor.SaiEscritor(this.id);
                sleep(this.delay);
            }
        }catch(InterruptedException e ){ return;}
    }
}

//--------------------------------------------------------
// Classe principal
class principal {
    public static final int  interacoes = 3;
    private static final int L = 2;
    private static final int E = 2;
    private static final int LE = 2;

    public static void main (String[] args) {
        int i;
        Monitor monitor = new Monitor();            // Monitor (objeto compartilhado entre leitores e escritores)
        Leitor[] l = new Leitor[L];       // Threads leitores
        Escritor[] e = new Escritor[E];   // Threads escritores
        LeitorEscritor [] le = new LeitorEscritor[LE]; //Threads leitures e escritores

        //inicia o log de saida
        System.out.println ("import verificaLE");
        System.out.println ("le = verificaLE.LE()");

        for (i=0; i<L; i++) {
            l[i] = new Leitor(i+1, (i+1)*500, monitor);
            l[i].start();
        }
        for (i=0; i<E; i++) {
            e[i] = new Escritor(i+1, (i+1)*500, monitor);
            e[i].start();
        }

        for (i = 0 ; i<LE; i++){
            le[i] = new LeitorEscritor( i+1,(i+1)*500, monitor);
            le[i].start();
        }
    }
}
