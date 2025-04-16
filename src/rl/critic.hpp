/*
 critic.hpp
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

#include "state.hpp"
#include "envoutcome.hpp"

namespace ia {
	namespace rl {
        class Critic {
        public:
            virtual void startEpisode(State *s)=0;
            virtual void endEpisode()=0;
            
            virtual double critique(EnvOutcome *eo, Action *m_ap)=0;

            virtual void reset()=0;
        };
    }
}