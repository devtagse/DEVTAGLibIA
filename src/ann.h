/*
 ann.h
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
 
#ifndef ANN_H_INCLUDED
#define ANN_H_INCLUDED

#include <vector>

class ANN {
private:
  std::vector<std::vector<double>> *m_input_weights;
  std::vector<std::vector<double>> *m_hidden_weights;

  std::vector<double> *m_output_inputlayer;
  std::vector<double> *m_output_hiddenlayer;
  std::vector<double> *m_output_outputlayer;

  std::vector<double> *m_input_hiddenlayer;
  std::vector<double> *m_input_outputlayer;

  std::vector<std::vector<double>> *m_last_dw_ih;
  std::vector<std::vector<double>> *m_last_dw_ho;

  int m_input_size;
  int m_hidden_size;
  int m_output_size;

  double m_alpha;
  double m_momentum;
  double m_cv;

  void update_weights(std::vector<std::vector<double>> *dw_ho, std::vector<std::vector<double>> *dw_ih);
  double calc_error(std::vector<double> output, int desired_ans, std::vector<double> &output_error);
  void calc_delta(std::vector<double> error, std::vector<std::vector<double>> *dw_ho, std::vector<std::vector<double>> *dw_ih);

  double actf(double v);
  double dactf(double v);

public:
  ANN(std::vector<std::vector<double>> *input_weights, std::vector<std::vector<double>> *hidden_weights,
      int input_size, int hidden_size, int output_size, double cv, double alpha, double momentum);
  virtual ~ANN();

  int output(std::vector<double> &input, std::vector<double> &output);
  double train(std::vector<double> &input, int desired_ans);

  void set_weights(std::vector<std::vector<double>> *input_weights, std::vector<std::vector<double>> *hidden_weights);
};

#endif // ANN_H_INCLUDED
