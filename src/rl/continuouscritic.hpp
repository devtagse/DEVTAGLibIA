/*
 anncontinuouscritic.hpp
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

#ifndef CONTINUOUSCRITIC_HPP
#define CONTINUOUSCRITIC_HPP

#include "critic.hpp"
#include "envoutcome.hpp"
#include "state.hpp"
#include "ann.h"
#include <stdexcept>
#include <vector>

namespace ia {
    namespace rl {
        /**
         * @brief Critico para estimar V(s).
         */
        class ContinuousCritic : public Critic {
        private:
            ANN *m_valueNet;   ///< Rede neural que estima V(s)
            int m_stateDim;    ///< Tamanho do vetor de estado
            int m_actionDim;

        public:
            /**
             * @brief Construtor
             * @param net       Rede neural (ex.: ANN) que implementa a função de valor
             * @param stateDim  Dimensão do estado
             */
            ContinuousCritic(ANN *net, int stateDim, int actionDim)
                : m_valueNet(net),
                m_stateDim(stateDim),
                m_actionDim(actionDim)
            {
                if (!m_valueNet)
                    throw std::runtime_error("ContinuousCritic: net nao pode ser nullptr.");
            }

            virtual ~ContinuousCritic() {}

            /**
             * @brief Início de episódio.
             */
            virtual void startEpisode(State *s) override {
            }

            /**
             * @brief Fim de episódio.
             */
            virtual void endEpisode() override {
            }

            /**
             * @brief Calcula a "crítica" para a transição. 
             *        Exemplo: TD(0) com V(s).
             */
            virtual double critique(EnvOutcome *eo, Action *m_ap) override {
                // TD-Error = r + gamma * Q(s',a') - Q(s,a)
                double gamma = 0.99; // fixo ou param
                // Pega Q(s',a')
                std::vector<double> Qsaprime(m_actionDim, 0.0);
                if (eo->m_op)
                    Qsaprime = value(eo->m_op, m_ap);
                std::vector<double> tdErrors(m_actionDim, 0.0);
                for (int i=0;i<m_actionDim;i++)
                    tdErrors[i] = eo->m_r + gamma * Qsaprime[i];

                // Pega Q(s,a)
                std::vector<double> Qsa = value(eo->m_o, eo->m_a);
                // Treina a rede para reduzir esse tdError
                // target = Vs + tdError
                // MSE:  (V(s) - target)^2
                std::vector<double> target(m_actionDim, 0.0);
                double targetValue = Vs + tdError;

                std::vector<double> stateVec = eo->m_o->to_vec();
                if ((int)stateVec.size() != m_stateDim)
                    throw std::runtime_error("ContinuousCritic: dimensao do estado difere.");
                m_valueNet->train(stateVec, target);

                return tdError;
            }

            /**
             * @brief Resetar algo, se precisar
             */
            virtual void reset() override {
            }

        private:
            /**
             * @brief Retorna Q(s,a) via rede neural
             */
            std::vector<double> value(State *s, Action *a) {
                std::vector<double> out(m_actionDim, 0.0);

                if (!s || !a) return out;
                
                std::vector<double> st = s->to_vec();
                if ((int)st.size() != m_stateDim) return out;
                
                if ((int)a->m_values.size() != m_actionDim) return out;
                for (int i=0;i<a->m_values.size();i++)
                    st.push_back(a->m_values[i]);

                m_valueNet->forward(st, out);
                return out;
            }
        };

    } // namespace rl
} // namespace ia

#endif // CONTINUOUSCRITIC_HPP
