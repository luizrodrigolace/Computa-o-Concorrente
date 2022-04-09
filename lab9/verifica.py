class Log:
    
    def __init__(self):
        self.bufferCheio = False
        self.posicaoConsumida = 0
        self.posicaoProduzida = 0
        self.bufferSize = 5
        
    def produtor(self, id, elemento, posicao):
        self.posicaoProduzida = posicao
        print(f"{id} produziu {elemento}.")

    def consumidor(self, id, elemento, posicao):
        self.posicaoConsumida = posicao
        print(f"{id} consumiu {elemento}.")
        self.verificaProblemas(id)

    def BufferCheio(self, id):
        self.bufferCheio = True

    def BufferVazio(self, id):
        self.bufferCheio = False

    def verificaProblemas(self, id):
        if self.bufferCheio == False:
            print("Erro! Consumindo quando o buffer não estava cheio!")
        if self.bufferCheio:
            if ((self.posicaoProduzida  + 1 ) % 10)> self.posicaoConsumida:
                print("Erro! Um produtor sobrescreveu um elemento.")
                print(str(self.posicaoProduzida) + " " + str(self.posicaoConsumida))
