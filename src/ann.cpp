/*
 ann.cpp
 Copyright (c) 2023 DEVTAG. Todos os direitos reservados.
  
 Este código faz parte do software SLAMduino, um produto desenvolvido
 pela DEVTAG. Todos os direitos reservados. A reprodução, distribuição,
 modificação ou uso deste software sem a devida autorização por escrito
 da DEVTAG é estritamente proibida.
  
 A DEVTAG não se responsabiliza por qualquer dano ou prejuízo causado
 pelo uso indevido deste software. Utilize-o por sua conta e risco.
  
 Para obter mais informações, entre em contato com a DEVTAG em:
 davi@devtag.com.br
 https://devtag.com.br
 */

#include <math.h>
#include <vector>

#include "ann.h"

ANN::ANN(std::vector<std::vector<double>> *input_weights, std::vector<std::vector<double>> *hidden_weights,
         int input_size, int hidden_size, int output_size, double cv, double alpha, double momentum) :
    m_input_size(input_size+1), m_hidden_size(hidden_size+1), m_output_size(output_size), m_cv(cv), m_alpha(alpha),
    m_momentum(momentum)
{
    m_input_weights = input_weights;
    m_hidden_weights = hidden_weights;
    m_last_dw_ih = new std::vector<std::vector<double>>(m_input_size, std::vector<double>(m_hidden_size-1));
    m_last_dw_ho = new std::vector<std::vector<double>>(m_hidden_size, std::vector<double>(m_output_size));
    m_output_inputlayer = new std::vector<double>(m_input_size);
    m_output_hiddenlayer = new std::vector<double>(m_hidden_size);
    m_output_outputlayer = new std::vector<double>(m_output_size);
    m_input_hiddenlayer = new std::vector<double>(m_input_size);
    m_input_outputlayer = new std::vector<double>(m_output_size);

    // Inicializa pesos (He)
    double he_hidden = sqrt(2.0/m_input_size);
    double he_output = sqrt(2.0/m_hidden_size);

    for (int i=0;i<m_input_size;i++) {
         for (int j=0;j<m_hidden_size-1;j++) // bias oculto não está conectado com a camada de entrada
	 		(*m_input_weights)[i][j] = he_hidden*(2*(rand()/(double)RAND_MAX)-1);
    }

    for (int i=0;i<m_hidden_size;i++) {
         for (int j=0;j<m_output_size;j++)
	 		(*m_hidden_weights)[i][j] = he_output*(2*(rand()/(double)RAND_MAX)-1);
    }

    // Inicializa neurônios BIAS
    (*m_output_inputlayer)[input_size] = 1;
    (*m_input_hiddenlayer)[hidden_size] = 1;
    (*m_output_hiddenlayer)[hidden_size] = 1;
}

ANN::~ANN()
{
}

void ANN::update_weights(std::vector<std::vector<double>> *dw_ho, std::vector<std::vector<double>> *dw_ih)
{
    for (int i = 0; i < m_hidden_size; i++) {
        for (int j = 0; j < m_output_size; j++) {
            // update weights
            (*m_hidden_weights)[i][j] +=
                (m_alpha * (*dw_ho)[i][j]) +
                (m_momentum * (*m_last_dw_ho)[i][j]);
            (*m_last_dw_ho)[i][j] = (*dw_ho)[i][j];
        }
    }

    for (int i = 0; i < m_input_size; i++) {
        for (int j = 0; j < m_hidden_size - 1; j++) { // bias oculto não está conectado com a camada de entrada
            // update weights
            (*m_input_weights)[i][j] +=
                (m_alpha * (*dw_ih)[i][j]) +
                (m_momentum * (*m_last_dw_ih)[i][j]);
            (*m_last_dw_ih)[i][j] = (*dw_ih)[i][j];
        }
    }
}

double ANN::calc_error(std::vector<double> output, int desired_ans, std::vector<double> &output_error)
{
    double error = 0;
    for (int i = 0; i < m_output_size; i++) {
        if (desired_ans == i)
            output_error[i] = 1 - output[i];
        else output_error[i] = 0 - output[i];
        error += pow(output_error[i], 2);
    }
    return error / 2;
}

void ANN::calc_delta(std::vector<double> error, std::vector<std::vector<double>> *dw_ho, std::vector<std::vector<double>> *dw_ih)
{
    std::vector<double> delta_output(m_output_size);
    std::vector<double> delta_hidden(m_hidden_size);

    // calcula o erro da camada de saída
    for (int i = 0; i < m_output_size; i++)
        delta_output[i] = error[i] * dactf((*m_input_outputlayer)[i]);

    // calcula o erro da camada oculta
    for (int i = 0; i < m_hidden_size; i++) {
        delta_hidden[i] = 0;
        for (int j = 0; j < m_output_size; j++)
            delta_hidden[i] += delta_output[j] * (*m_hidden_weights)[i][j];
        delta_hidden[i] *= dactf((*m_input_hiddenlayer)[i]);
    }
    for (int i = 0; i < m_hidden_size; i++) {
        for (int j = 0; j < m_output_size; j++)
            (*dw_ho)[i][j] += delta_output[j] * (*m_output_hiddenlayer)[i];
    }

    // calcula o erro da camada de entrada
    for (int i = 0; i < m_input_size; i++) {
        for (int j = 0; j < m_hidden_size - 1; j++) // bias oculto não está conectado com a camada de entrada
            (*dw_ih)[i][j] += delta_hidden[j] * (*m_output_inputlayer)[i];
    }
}
 
int ANN::output(std::vector<double> &input, std::vector<double> &output)
{
    // entrada da rede
	for (int i=0;i<m_input_size-1;i++)
	    (*m_output_inputlayer)[i] = input[i];

    // calcula saída de cada camada
    for (int i = 0; i < m_hidden_size - 1; i++) { // não calcula a entrada do neurônio bias da camada oculta
        (*m_input_hiddenlayer)[i] = 0;
        for (int j = 0; j < m_input_size; j++)
            (*m_input_hiddenlayer)[i] += (*m_output_inputlayer)[j] * (*m_input_weights)[j][i];
        (*m_output_hiddenlayer)[i] = actf((*m_input_hiddenlayer)[i]);
    }

    for (int i = 0; i < m_output_size; i++) {
        (*m_input_outputlayer)[i] = 0;
        for (int j = 0; j < m_hidden_size; j++)
            (*m_input_outputlayer)[i] += (*m_output_hiddenlayer)[j] * (*m_hidden_weights)[j][i];
        (*m_output_outputlayer)[i] = actf((*m_input_outputlayer)[i]);
    }

    // determina o neurônio de saída com maior valor de saída
    float max = (*m_output_outputlayer)[0];
    output[0] = (*m_output_outputlayer)[0];
    int idx = 0;
    for (int i = 1; i < m_output_size; i++) {
        output[i] = (*m_output_outputlayer)[i];
        if ((*m_output_outputlayer)[i] > max) {
            max = (*m_output_outputlayer)[i];
            idx = i;
        }

    }

    return idx;
}

double ANN::train(std::vector<double> &input, int desired_ans)
{
    std::vector<double> output_outputlayer(m_output_size);
    output(input, output_outputlayer);

	double e = 0;
	std::vector<double> error(m_output_size);
	e = calc_error(output_outputlayer, desired_ans, error);

    std::vector<std::vector<double>> *dw_ih = new std::vector<std::vector<double>>(m_input_size, std::vector<double>(m_hidden_size));
    std::vector<std::vector<double>> *dw_ho = new std::vector<std::vector<double>>(m_hidden_size, std::vector<double>(m_output_size));
    calc_delta(error, dw_ho, dw_ih);
	update_weights(dw_ho, dw_ih);
    delete dw_ih;
    delete dw_ho;

    return e;
}

double ANN::actf(double v)
{
    return ((v>=0)?v:m_cv*v);
}

double ANN::dactf(double v)
{
    return ((v>=0)?1:m_cv);
}

void ANN::set_weights(std::vector<std::vector<double>> *input_weights, std::vector<std::vector<double>> *hidden_weights)
{
    // check if the weights have the same size
    if (input_weights->size() != m_input_size || hidden_weights->size() != m_hidden_size || (*hidden_weights)[0].size() != m_output_size)
        return;
    m_input_weights = input_weights;
    m_hidden_weights = hidden_weights;
}
