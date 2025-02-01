// Inicialização das bibliotecas padrão para funcionamento do código
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"

// Definição da matriz de LEDs
#define NUM_LEDS 25
// Definição do pino de controle da matriz de LEDs
#define PIN 7
// Definição dos botões e LEDs RGB
#define button_A 5
#define button_B 6
#define led_R 13
#define led_G 11
#define led_B 12

// Prototipo da função de interrupção
static void botao_pressionado(uint gpio, uint32_t events); 

// Armazena o tempo do último evento (em microssegundos)
static volatile uint32_t ultimo_evento = 0; 

// Definição dos desenhos na matriz de LEDs
double num_1[NUM_LEDS] = 
        
    {0.2, 0.2, 0.2, 0.2, 0.2,
     0.0, 0.0, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.2, 0.0, 0.2,
     0.0, 0.2, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.2, 0.0, 0.0};

double num_2[NUM_LEDS] = 
        
    {0.2, 0.2, 0.2, 0.2, 0.2,
     0.2, 0.0, 0.0, 0.0, 0.0,
     0.2, 0.2, 0.2, 0.2, 0.2,
     0.0, 0.0, 0.0, 0.0, 0.2,
     0.2, 0.2, 0.2, 0.2, 0.2};

double num_3[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,};

double num_4[NUM_LEDS] = 
        
    {0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0};

double num_5[NUM_LEDS] = 
        
   {0.2, 0.2, 0.2, 0.2, 0.2,
    0.0, 0.0, 0.0, 0.0, 0.2,
    0.2, 0.2, 0.2, 0.2, 0.2,
    0.2, 0.0, 0.0, 0.0, 0.0,
    0.2, 0.2, 0.2, 0.2, 0.2};

double num_6[NUM_LEDS] = 
        
    {0.2, 0.2, 0.2, 0.2, 0.2,
     0.2, 0.0, 0.0, 0.0, 0.2,
     0.2, 0.2, 0.2, 0.2, 0.2,
     0.2, 0.0, 0.0, 0.0, 0.0,
     0.2, 0.2, 0.2, 0.2, 0.2};

double num_7[NUM_LEDS] = 
        
    {0.0, 0.2, 0.0, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.2, 0.2, 0.2, 0.0, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double num_8[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};

double num_9[NUM_LEDS] = 
        
    {0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.0, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0,
     0.0, 0.2, 0.0, 0.2, 0.0,
     0.0, 0.2, 0.2, 0.2, 0.0};


// Variável para armazenar o desenho atual que pode mudar a qualquer momento
volatile uint8_t desenho_atual = 0; 
// Vetor de desenhos para serem exibidos na matriz de LEDs
double* desenhos[] = {num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9};

// Definição da intensidade de cada cor na matriz de LEDs
double matrix_rgb(double r, double g, double b) {
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Rotina para acionar a matriz de LEDs - WS2812B
void animacao(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b) {
    for (int16_t i = 0; i < NUM_LEDS; i++) {
        valor_led = matrix_rgb(desenho[i] * r, desenho[i] * g, desenho[i] * b);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}



void blink_red(){ // Função para piscar o LED vermelho 5 vezes consecutivas por segundo
    for (int i = 0; i < 5; i++)
    {
        gpio_put(led_R, 1);
        sleep_ms(100);
        gpio_put(led_R, 0);
        sleep_ms(100);
    }
    
}




int main(){
    stdio_init_all();
    PIO pio = pio0; // Inicialização do pio
   

    bool verificado = set_sys_clock_khz(128000, false); // Configuração do clock do sistema 
    

    // Inicialização do LED VERMELHO
    gpio_init(led_R);
    gpio_set_dir(led_R, GPIO_OUT);
    gpio_put(led_R, 0);

    // Inicialização do botão A
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    // Inicialização do botão B
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);

    // Configuração PIO

    // Adiciona o programa ao pio às maquinas de estado
    uint offset = pio_add_program(pio, &Atv_wilton_program);
    // Reivindica uma máquina de estado livre para executar o programa PIO
    uint sm = pio_claim_unused_sm(pio, true); 
    // Configura a SM para executar o programa PIO
    Atv_wilton_program_init(pio, sm, offset, PIN, 800000, false); 

    // Configuração da interrupção com callback para botão A
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &botao_pressionado); 

    // Configuração da interrupção com callback para botão B
    gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, true, &botao_pressionado);

    printf("Sistema iniciado\n");

    while (true)
    {
        
        blink_red();
        animacao(desenhos[desenho_atual], 0, pio, sm, 0.2, 0.2, 0.2);
        sleep_ms(100); // Delay para evitar sobrecarga
        
    }
    
}
    
// Função de interrupção
void botao_pressionado(uint gpio, uint32_t events)
{
   
    // Verifica se o tempo entre os eventos é maior que 300ms
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Se o tempo entre os eventos for maior que 300ms, o evento é registrado
    if (current_time - ultimo_evento > 300000) 
    {
        // Atualiza o tempo do último evento para o tempo atual
        ultimo_evento = current_time;
        
        // Verifica qual botão foi pressionado e altera o desenho atual
        if (gpio == button_A)
        {   
            // Incrementa o desenho atual e cria um ciclo de 0 a 8 
            desenho_atual = (desenho_atual + 1) % 9; 
            printf("Botão A pressionado. Desenho atual: %d\n", desenho_atual);
        } 
        else if (gpio == button_B)
        {   
            // Decrementa o desenho atual e cria um ciclo de 8 a 0
            desenho_atual = (desenho_atual == 0) ? 8 : (desenho_atual - 1); 
            printf("Botão B pressionado. Desenho atual: %d\n", desenho_atual);
        }
   }
}
        