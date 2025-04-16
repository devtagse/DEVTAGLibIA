/*
 actorcritic.hpp
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

#ifndef ACTOR_CRITIC_H
#define ACTOR_CRITIC_H

#include <deque>
#include <memory>
#include <vector>
#include <stdexcept>
#include <limits>

#include "actor.hpp"
#include "critic.hpp"
#include "episode.hpp"
#include "env.hpp"

namespace ia {
	namespace rl {
        class ActorCritic {
        protected:
            // Ponteiros inteligentes para o Actor e o Critic
            Actor *m_actor;
            Critic *m_critic;

        public:
            // Construtores
            ActorCritic(Actor *actor, Critic *critic)
                : m_actor(actor), m_critic(critic)
            {
            }

            // Implementação do LearningAgent
            // 1) Sem limite de passos
            virtual Episode runLearningEpisode(Env* env) {
                return this->runLearningEpisode(env, -1);
            }

            // 2) Com limite de passos
            virtual Episode runLearningEpisode(Env* env, int maxSteps) {
                // estado inicial
                State *initialState = env->curr_obs();
                Episode episode(initialState);
                State *curState = initialState;

                m_critic->startEpisode(curState);
                m_actor->startEpisode(curState);

                int timeSteps = 0;
                while(!env->is_terminal() && (timeSteps < maxSteps || maxSteps == -1)) {
                    // Escolhe ação
                    Action *action = m_actor->action(curState);
                    // Executa ação
                    EnvOutcome *eo = env->exec_act(action);
                    // Registra transição
                    episode.transition(*eo);
                    // Crítica do critic
                    double critique = m_critic->critique(eo);
                    // Atualiza o ator
                    m_actor->update(eo, critique);
                    // Move para o próximo estado
                    curState = env->curr_obs();
                    timeSteps++;
                }

                m_critic->endEpisode();
                m_actor->endEpisode();

                return episode;
            }
        };
    }
}

#endif // ACTOR_CRITIC_H
