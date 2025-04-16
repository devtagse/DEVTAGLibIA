/*
 annpolicygrad.hpp
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

#ifndef ANNPOLICYGRAD_H
#define ANNPOLICYGRAD_H

#include "ann.h"
#include <vector>
#include <cmath>

/**
 * @brief Classe derivada de ANN que sobrescreve train(...)
 *        para usar Policy Gradient.
 * 
 * Neste caso, o segundo parâmetro de train(...) (chamado "grads") 
 * será interpretado como o gradiente externo da perda em relação 
 * às saídas da rede (dLdOut).
 */
class ANNPolicyGrad : public ANN {
public:
    // Reaproveita o construtor da base (ANN)
    // que recebe (layerSizes, cv, alpha, momentum).
    using ANN::ANN;

    virtual ~ANNPolicyGrad() {}

    /**
     * @brief Sobrescreve train(...) para usar o gradiente externo.
     * @param input vetor de entrada
     * @param grads gradiente da perda em relação às saídas (dLdOut),
     *              mesmo tamanho que a camada de saída.
     * @return algum valor que represente o "custo" ou 
     *         a norma do gradiente, se desejar.
     */
    virtual double train(const std::vector<double> &input,
                         const std::vector<double> &grads) override
    {
        // 1) Forward pass para atualizar as ativações internas
        std::vector<double> out(m_layerSizes.back());
        forward(input, out);

        // 2) Cria o "erro" local para reaproveitar a lógica de calc_delta(...) 
        //    da classe base. Em vez de (target - out), usamos dLdOut = grads.
        std::vector<double> error = grads; // "erro" = gradiente externo

        // 3) Aloca vetor de gradientes de pesos (dw) 
        //    do mesmo tamanho que m_weights da classe base.
        int L = m_layerSizes.size() - 1;
        std::vector<std::vector<std::vector<double>>> dw(L);
        for (int l = 0; l < L; l++) {
            int inSz  = m_layerSizes[l] + 1; 
            int outSz = m_layerSizes[l+1];
            dw[l].resize(inSz, std::vector<double>(outSz, 0.0));
        }

        // 4) Retropropagação (chamando o método base calc_delta) 
        //    que espera "error" na camada de saída = error[k].
        calc_delta(error, dw);

        // 5) Atualiza pesos (método base)
        update_weights(dw);

        // Exemplo: retornar a norma do gradiente 
        double normGrad = 0.0;
        for (double g : grads)
            normGrad += g*g;
        return std::sqrt(normGrad);
    }
};

#endif // ANNPOLICYGRAD_H
