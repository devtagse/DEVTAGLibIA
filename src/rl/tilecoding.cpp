/*
 tilecoding.cpp
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

#include "tilecoding.hpp"

using namespace ia::rl;

StateFeature::StateFeature() {}

StateFeature::StateFeature(int id, double value) : m_id(id), m_value(value) {}

StateFeature::StateFeature(const StateFeature &state_feature) : m_id(state_feature.m_id), m_value(state_feature.m_value) {}

Tile::Tile() {}

Tile::Tile(std::vector<int> &tiled_vector, std::vector<bool> &dim_mask) : m_tiled_vector(tiled_vector)
{
    m_hash_code = 0;
    for (int i = 0; i < tiled_vector.size(); i++) {
        if (dim_mask[i])
            m_hash_code = 31 * m_hash_code + tiled_vector[i];
    }
}

Tile::Tile(const Tile &tile) : m_tiled_vector(tile.m_tiled_vector), m_hash_code(tile.m_hash_code) {}

bool Tile::operator <(const Tile& other) const
{
    return m_hash_code < other.m_hash_code;
}

bool Tile::operator ==(const Tile& other) const
{
    if (m_tiled_vector.size() != other.m_tiled_vector.size())
        return false;
    for (int i = 0; i < m_tiled_vector.size(); i++) {
        if (m_tiled_vector[i] != other.m_tiled_vector[i])
            return false;
    }
    return true;
}

Tiling::Tiling() {}

Tiling::Tiling(std::vector<double> &widths, std::vector<double> &offset) : m_widths(widths), m_offset(offset), m_dim_mask(widths.size())
{
    for (int i = 0; i < widths.size(); i++)
        m_dim_mask[i] = true;
}

Tiling::Tiling(std::vector<double> &widths, std::vector<double> &offset, std::vector<bool> &dim_mask) : m_widths(widths), m_offset(offset), m_dim_mask(dim_mask) {}

Tiling::Tiling(const Tiling &tiling) : m_widths(tiling.m_widths), m_offset(tiling.m_offset), m_dim_mask(tiling.m_dim_mask) {}

Tile Tiling::get_tile(std::vector<double> input)
{
    std::vector<int> tiled_vector(input.size());
    for (int i = 0; i < input.size(); i++)
    {
        if (m_dim_mask[i])
            tiled_vector[i] = (int)floor((input[i] - m_offset[i]) / m_widths[i]);
        else
            tiled_vector[i] = 0;
    }
    return Tile(tiled_vector, m_dim_mask);
}

int TileCoding::get_or_gen_feature(std::map<Tile, int> &tile_map, Tile &tile)
{
    int stored;
    if (tile_map.find(tile) == tile_map.end())
    {
        stored = m_feature_id++;
        tile_map[tile] = stored;
    }
    else
        stored = tile_map[tile];

    return stored;
}

std::vector<double> TileCoding::rand_offset(std::vector<bool> &dim_mask, std::vector<double> &widths)
{
    std::vector<double> offset(dim_mask.size());

    for (int i = 0; i < offset.size(); i++)
    {
        if (dim_mask[i])
            offset[i] = m_distribution(m_gen) * widths[i];
        else
            offset[i] = 0.;
    }
    return offset;
}

TileCoding::TileCoding() : m_distribution(0, 1), m_feature_id(0) {}

TileCoding::TileCoding(const TileCoding &tile_coding) : m_feature_id(tile_coding.m_feature_id), m_tilings(tile_coding.m_tilings), m_state_features(tile_coding.m_state_features),
                                            m_gen(tile_coding.m_gen), m_distribution(tile_coding.m_distribution) {}

void TileCoding::add_tiling(std::vector<bool> &dim_mask, std::vector<double> &widths, int num_tilings)
{
    for (int i = 0; i < num_tilings; i++)
    {
        m_state_features.push_back(std::map<Tile, int>());
        std::vector<double> offset = rand_offset(dim_mask, widths);
        m_tilings.push_back(Tiling(widths, offset, dim_mask));
    }
}

std::vector<StateFeature> TileCoding::features(ia::rl::State *s)
{
    std::vector<double> input = s->to_vec();
    std::vector<StateFeature> features;
    for (int i = 0; i < m_tilings.size(); i++)
    {
        Tile tile = m_tilings[i].get_tile(input);
        int f = get_or_gen_feature(m_state_features[i], tile);
        StateFeature sf(f, 1.);
        features.push_back(sf);
    }
    return features;
}

int TileCoding::num_features()
{
    return m_feature_id;
}