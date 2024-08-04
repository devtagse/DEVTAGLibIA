/*
 gdsarsalambda.cpp
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

#include "gdsarsalambda.hpp"

using namespace ia::rl;

GDSarsaLambda::GDSarsaLambda(double alpha, double lambda, double gamma, double E, double min_lambda, bool replace_traces, LinearFA *vfa) :
    m_alpha(alpha), m_lambda(lambda), m_gamma(gamma), m_E(E), m_min_lambda(min_lambda), m_replace_traces(replace_traces),
    m_vfa(vfa), m_distribution(0, 1) {}

Episode *GDSarsaLambda::run_learning(Env *env, int max_steps)
{
    State *curr_s = env->curr_obs();
    Episode *ea = new Episode(curr_s);

    m_curr_step = 0;
    std::map<int, double> traces;
    std::vector<int> delete_traces;

    Action *action = egreedy_action(env, curr_s);
    while (!env->is_terminal() && (m_curr_step < max_steps || max_steps == -1)) {
        // get Q-value and gradient
        double curr_Q = m_vfa->evaluate(*curr_s, *action);
        std::map<int, double> gradient = m_vfa->gradient(curr_s, action);

        EnvOutcome *eo = env->exec_act(action);
        State *next_s = eo->m_op;

        // determine next Q-value for outcome state
        Action *next_a = egreedy_action(env, next_s);
        double next_Q = 0.;
        if (!eo->m_terminated)
            next_Q = m_vfa->evaluate(*next_s, *next_a);

        // manage option specifics
        double r = eo->m_r;
        m_curr_step++;
        ea->transition(action, next_s, r);

        // compute function delta
        double delta = r + (m_gamma * next_Q) - curr_Q;

        // manage traces
        std::map<int, double> ofg = m_vfa->gradient(curr_s, action);
        for (std::map<int, double>::iterator it = ofg.begin(); it != ofg.end(); ++it) {
            if (m_replace_traces)
                traces[it->first] = 1.;
            else
                traces[it->first] += 1;
        }

        for (std::map<int, double>::iterator it = traces.begin(); it != traces.end(); ++it) {
            m_vfa->m_weights[it->first] = m_vfa->get_weight(it->first) + m_alpha * delta * traces[it->first];
            // now decay and delete from tracking if too small
            traces[it->first] *= m_lambda * m_gamma;
            if (traces[it->first] < m_min_lambda)
                delete_traces.push_back(it->first);
        }

        // delete traces marked for deletion
        for (int i : delete_traces)
            traces.erase(i);

        // move on
        curr_s = next_s;
        action = next_a;
    }

    return ea;
}

Action *GDSarsaLambda::egreedy_action(Env *env, State *curr_s)
{
    std::vector<Action *> actions = env->applicable_actions(curr_s);
    Action *best = greedy_action(env, curr_s);
    if (m_distribution(m_gen) > m_E)
        return best;
    else
    {
        int rnd_a = round((actions.size() - 1) * m_distribution(m_gen));
        return actions[rnd_a];
    }
}

Action *GDSarsaLambda::greedy_action(Env *env, State *curr_s)
{
    std::vector<Action *> actions = env->applicable_actions(curr_s);
    Action *best = actions[0];
    double vsa = m_vfa->evaluate(*curr_s, *best);
    for (Action *a : actions) {
        double v = m_vfa->evaluate(*curr_s, *a);
        if (v > vsa) {
            best = a;
            vsa = v;
        }
    }
    return best;
}
