/*
 anncontinuousactor.hpp
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

#ifndef CONTINUOUSACTOR_HPP
#define CONTINUOUSACTOR_HPP

#include "actor.hpp"
#include "envoutcome.hpp"
#include "action.hpp"
#include "annpolicygrad.hpp"
#include <stdexcept>
#include <vector>

namespace ia {
    namespace rl {
        /**
         * @brief Ator para ações contínuas, usando ANNPolicyGrad
         *        ou outra rede neural para mapear estado -> ação contínua.
         */
        class ContinuousActor : public Actor {
        private:
            ANNPolicyGrad *m_policyNet; ///< Ponteiro para a rede de política (ex.: ANN que usa policy gradient)
            int m_stateDim;   ///< Dimensão do estado (tamanho do vetor de entrada da rede)
            int m_actionDim;  ///< Dimensão do vetor de ação (saída da rede)

        public:
            /**
             * @brief Construtor
             * @param policyNet  Rede neural (ANN) que implementa a política
             * @param stateDim   Tamanho do vetor de estado
             * @param actionDim  Tamanho do vetor de ação contínua
             */
            ContinuousActor(ANNPolicyGrad *policyNet,
                            int stateDim,
                            int actionDim)
                : m_policyNet(policyNet),
                m_stateDim(stateDim),
                m_actionDim(actionDim)
            {
                if (!m_policyNet)
                    throw std::runtime_error("ContinuousActor: policyNet nao pode ser nulo.");
            }

            virtual ~ContinuousActor() {}

            /**
             * @brief Chamado no início de cada episódio. 
             *        Pode ser usado para resetar buffers, se necessário.
             */
            virtual void startEpisode(State *s) override {
            }

            /**
             * @brief Chamado no fim de cada episódio.
             */
            virtual void endEpisode() override {
            }

            /**
             * @brief Decide a ação contínua com base no estado atual, usando a rede de política.
             */
            virtual Action* action(State *s) override {
                // 1) Obter representação do estado como std::vector<double>
                //    por exemplo, s->asVector() (depende de como seu State é implementado).
                std::vector<double> stateVec = s->to_vec();
                if ((int)stateVec.size() != m_stateDim)
                    throw std::runtime_error("ContinuousActor: dimensão do estado não confere.");

                // 2) Faz forward na rede para obter ação
                std::vector<double> actVec(m_actionDim, 0.0);
                m_policyNet->forward(stateVec, actVec);

                // 3) Cria e retorna a ContinuousAction
                return new Action("CONTINUOUS_ACTION", actVec);
            }

            /**
             * @brief Atualiza o ator usando a "crítica" vinda do Critic, se estiver fazendo
             *        algo estilo Policy Gradient.
             *
             * @param eo       EnvOutcome com info de transição (estado, ação, reward, etc.)
             * @param critique Valor do Critic (pode ser advantage, TD-error, etc.)
             */
            virtual void update(EnvOutcome *eo, double critique) override {
                // EXEMPLO fictício:
                // std::vector<double> dLdOut(m_actionDim, 0.0);
                // for (int k = 0; k < m_actionDim; k++) {
                //    dLdOut[k] = critique * <derivada da ação w.r.t. saida da rede ...>;
                // }
                // m_policyNet->train(stateVec, dLdOut);
            }

            virtual void reset() override {
            }
        };

    } // namespace rl
} // namespace ia

#endif // CONTINUOUSACTOR_HPP
