/*
 rl_env.h
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
 
#ifndef RL_ENV_H_INCLUDED
#define RL_ENV_H_INCLUDED

#include <vector>

class RLEnv {
public:
    RLEnv() { }

    virtual ~RLEnv() { }

    virtual void act(int a)=0;

    virtual std::vector<double> getState()=0;

    virtual double getReward()=0;

    virtual std::vector<int> getApplicableActions(std::vector<double> state)=0;

    virtual bool isEndEpisode()=0;

    virtual void reset()=0;
};

#endif // RL_ENV_H_INCLUDED