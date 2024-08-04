/*
 episode.cpp
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

#include "episode.hpp"

using namespace ia::rl;

Episode::Episode() {}

Episode::Episode(State *init_state)
{
    add_state(init_state);
}

Episode::Episode(const Episode &episode) :
    m_state_seq(episode.m_state_seq), m_action_seq(episode.m_action_seq),
    m_reward_seq(episode.m_reward_seq)
{
}

Episode::~Episode()
{
    for (State *s : m_state_seq)
        delete s;
}

void Episode::add_state(State *s)
{
    m_state_seq.push_back(s);
}

void Episode::add_action(Action *a)
{
    m_action_seq.push_back(a);
}

void Episode::add_reward(double r)
{
    m_reward_seq.push_back(r);
}

void Episode::transition(Action *curr_action, State *next_state, double r)
{
    m_state_seq.push_back(next_state);
    m_action_seq.push_back(curr_action);
    m_reward_seq.push_back(r);
}

void Episode::transition(EnvOutcome eo)
{
    m_state_seq.push_back(eo.m_op);
    m_action_seq.push_back(eo.m_a);
    m_reward_seq.push_back(eo.m_r);
}

int Episode::num_steps()
{
    return m_state_seq.size();
}

double Episode::discounted_return(double gamma)
{
    double discount = 1.;
    double sum = 0.;
    for (double r : m_reward_seq)
    {
        sum += discount * r;
        discount *= gamma;
    }
    return sum;
}