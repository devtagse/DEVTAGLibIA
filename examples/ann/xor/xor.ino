/*
 xor.ino - Exemplo de como utilizar redes mlp no arduíno.
 Copyright (c) 2023 DEVTAG. Todos os direitos reservados.
  
 Este código faz parte do software SLAMduino, um produto desenvolvido
 pela DEVTAG. Todos os direitos reservados. A DEVTAG não se responsabiliza
 por qualquer dano ou prejuízo causado pelo uso indevido deste software.
 Utilize-o por sua conta e risco.
  
 Para obter mais informações, entre em contato com a DEVTAG em:
 davi@devtag.com.br
 https://devtag.com.br
 */

#include <DEVTAGLibIA.h>

#define ANN_INPUT_WEIGHTS { {-1.67207,-2.32}, {1.67208,2.32002}, {4.88071e-05,-2.31999} }
#define ANN_HIDDEN_WEIGHTS { {-1.5948,1.59613}, {2.87348,-2.87594}, {2.66665,-1.66884} }

#define RELU_V 0.25

#define ANN_NUM_HIDDEN_UNITS 2
#define ANN_NUM_OUTPUT 2
#define ANN_NUM_INPUTS 2

std::vector<double> *ann_input = new std::vector<double>(ANN_NUM_INPUTS);

std::vector<double> max_values = { 1, 1 };
std::vector<double> min_values = { 0, 0 };

std::vector<std::vector<double>> *ann_input_weights = new std::vector<std::vector<double>> ANN_INPUT_WEIGHTS;
std::vector<std::vector<double>> *ann_hidden_weights = new std::vector<std::vector<double>> ANN_HIDDEN_WEIGHTS;
ANN *ann;

static std::vector<double> ann_output(ANN_NUM_OUTPUT);

void setup()
{
	ann = new ANN(ann_input_weights, ann_hidden_weights, ANN_NUM_INPUTS, ANN_NUM_HIDDEN_UNITS, ANN_NUM_OUTPUT, RELU_V, 0, 0);
}

void loop()
{
	int ans;

	Serial.println("A\tB\tA^B");
	
	Serial.print("0\t0\t");
	ann_input[0] = 0;
	ann_input[1] = 0;
	ans = ann_touch->output(*ann_input, ann_output);
	Serial.println(ans);
	
	Serial.print("0\t1\t");
	ann_input[0] = 0;
	ann_input[1] = 1;
	ans = ann_touch->output(*ann_input, ann_output);
	Serial.println(ans);
	
	Serial.print("1\t0\t");
	ann_input[0] = 1;
	ann_input[1] = 0;
	ans = ann_touch->output(*ann_input, ann_output);
	Serial.println(ans);
	
	Serial.print("1\t1\t");
	ann_input[0] = 1;
	ann_input[1] = 1;
	ans = ann_touch->output(*ann_input, ann_output);
	Serial.println(ans);
	
	while(1) ;
}
