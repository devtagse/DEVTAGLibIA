/*
 linearfa.cpp
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

#include "linearfa.hpp"

using namespace ia::rl;

FeaturesMap::FeaturesMap() {}

FeaturesMap::FeaturesMap(const FeaturesMap &features_map) : m_features_map(features_map.m_features_map) {}

int FeaturesMap::get_or_create(int from, int &feature_id)
{
    int to = 0;
    if (m_features_map.find(from) == m_features_map.end()) {
        to = feature_id++;
        m_features_map[from] = to;
    }
    else
        to = m_features_map[from];
    return to;
}

int CrossProductFeatures::action_feature(Action &a, int from)
{
    return m_action_features[a.m_num].get_or_create(from, m_feature_id);
}

CrossProductFeatures::CrossProductFeatures(TileCoding sfeatures) : m_sfeatures(sfeatures), m_feature_id(0) {}

CrossProductFeatures::CrossProductFeatures(const CrossProductFeatures &cross_pfeatures) : m_sfeatures(cross_pfeatures.m_sfeatures), m_action_features(cross_pfeatures.m_action_features),
                                                                                          m_feature_id(cross_pfeatures.m_feature_id) {}

std::vector<StateFeature> CrossProductFeatures::features(State *s, Action &a)
{
    std::vector<StateFeature> sfs = m_sfeatures.features(s);
    std::vector<StateFeature> safs;
    safs.reserve(sfs.size());

    for (StateFeature sf : sfs) {
        StateFeature saf(action_feature(a, sf.m_id), sf.m_value);
        safs.push_back(saf);
    }
    return safs;
}

int CrossProductFeatures::num_features()
{
    return m_sfeatures.num_features() * m_feature_id;
}

LinearFA::LinearFA() : m_safeatures(TileCoding()), m_default_weight(0) {}

LinearFA::LinearFA(TileCoding tilecoding) : m_safeatures(tilecoding), m_default_weight(0.) {}

LinearFA::LinearFA(TileCoding tilecoding, double default_weight) : m_safeatures(tilecoding), m_default_weight(default_weight) {}

double LinearFA::evaluate(State &s, Action &a)
{
    std::vector<StateFeature> features = m_safeatures.features(&s, a);
    double val = 0.;
    for (StateFeature sf : features) {
        double prod = sf.m_value * get_weight(sf.m_id);
        val += prod;
    }
    m_curr_value = val;
    m_curr_gradient.clear();
    m_curr_features = features;

    m_last_state = &s;
    m_last_action = &a;

    return val;
}

std::map<int, double> LinearFA::gradient(State *s, Action *a)
{
    std::vector<StateFeature> features;

    if (*m_last_state == *s && *m_last_action == *a) {
        if (m_curr_gradient.size() > 0)
            return m_curr_gradient;
        features = m_curr_features;
    }
    else features = m_safeatures.features(s, *a);

    std::map<int, double> gd;
    for (StateFeature sf : features)
        gd[sf.m_id] = sf.m_value;

    m_curr_gradient = gd;
    m_last_state = s;
    m_last_action = a;
    m_curr_features = features;

    return gd;
}

int LinearFA::num_param()
{
    return m_weights.size();
}

double LinearFA::get_weight(int weight_id)
{
    if (m_weights.find(weight_id) == m_weights.end()) {
        m_weights[weight_id] = m_default_weight;
        return m_default_weight;
    }
    return m_weights[weight_id];
}

void LinearFA::reset_params()
{
    m_weights.clear();
}
