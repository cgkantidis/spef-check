#ifndef SPEF_WRITE_HPP
#define SPEF_WRITE_HPP

#include "spef_structs.hpp"

std::string_view get_connection_type_sv(ConnType type) {
  if (type == ConnType::ExternalConnection) {
    return "P";
  }
  if (type == ConnType::InternalConnection) {
    return "I";
  }
  throw std::runtime_error("Unknown connection type");
};

std::string_view get_direction_type_sv(DirType type) {
  if (type == DirType::Input) {
    return "I";
  }
  if (type == DirType::Output) {
    return "O";
  }
  if (type == DirType::Bidirectional) {
    return "B";
  }
  throw std::runtime_error("Unknown direction type");
};

std::ostream &operator<<(std::ostream &os, Capacitances const &caps) {
  bool is_first = true;
  for (cap_t cap : caps.m_caps) {
    std::print(os, "{}{}", (is_first ? ' ' : ':'), cap);
    is_first = false;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, Thresholds const &thresholds) {
  bool is_first = true;
  for (thresh_t thresh : thresholds.m_thresh) {
    std::print(os, "{}{}", (is_first ? ' ' : ':'), thresh);
    is_first = false;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, CoordinatesAttr const &coord) {
  std::print(os, " *C {} {}", coord.m_coord.x, coord.m_coord.y);
  return os;
}

std::ostream &operator<<(std::ostream &os, CapLoadAttr const &cap_load) {
  std::print(os, " *L");
  os << cap_load.m_cap;
  return os;
}

std::ostream &operator<<(std::ostream &os, SlewsAttr const &slews) {
  std::print(os, " *S");
  os << slews.m_cap1 << slews.m_cap2 << slews.m_thresh1 << slews.m_thresh2;
  return os;
}

std::ostream &
operator<<(std::ostream &os, DrivingCellAttr const &driving_cell) {
  std::print(os, " *D {}", driving_cell.m_cell);
  return os;
}

std::ostream &operator<<(std::ostream &os, ConnAttr const *conn_attr) {
  switch (conn_attr->m_type) {
  case ConnAttrType::Coordinates: {
    os << *static_cast<CoordinatesAttr const *>(conn_attr);
    break;
  }
  case ConnAttrType::CapLoad: {
    os << *static_cast<CapLoadAttr const *>(conn_attr);
    break;
  }
  case ConnAttrType::Slews: {
    os << *static_cast<SlewsAttr const *>(conn_attr);
    break;
  }
  case ConnAttrType::DrivingCell: {
    os << *static_cast<DrivingCellAttr const *>(conn_attr);
    break;
  }
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, DNet const &d_net) {
  std::println(os, "\n*D_NET {} {}", d_net.m_name, d_net.m_total_cap);
  if (d_net.m_routing_conf != 0) {
    std::println(os, "*V {}", d_net.m_routing_conf);
  }
  std::println(os, "*CONN");
  for (auto const &connection : d_net.m_conns) {
    std::print(
        os,
        "*{} {} {}",
        get_connection_type_sv(connection.m_type),
        connection.m_name,
        get_direction_type_sv(connection.m_direction));
    for (auto const &conn_attr : connection.m_conn_attrs) {
      os << conn_attr.get();
    }
    std::println(os, "");
  }
  for (auto const &node : d_net.m_nodes) {
    // TODO: change ':' with spef pin delimiter
    std::println(
        os,
        "*N {}:{} {} {}",
        d_net.m_name,
        node.m_name,
        node.m_coords->m_coord.x,
        node.m_coords->m_coord.y);
  }
  if (!d_net.m_ground_caps.empty() || !d_net.m_coupling_caps.empty()) {
    // TODO: add connection attributes
    std::size_t cap_idx = 1;
    std::println(os, "*CAP");
    for (auto const &ground_cap : d_net.m_ground_caps) {
      std::println(
          os,
          "{} {} {}",
          cap_idx++,
          ground_cap.m_node,
          ground_cap.m_cap);
    }
    for (auto const &coupling_cap : d_net.m_coupling_caps) {
      std::println(
          os,
          "{} {} {} {}",
          cap_idx++,
          coupling_cap.m_node1,
          coupling_cap.m_node2,
          coupling_cap.m_cap);
    }
  }
  if (!d_net.m_resistances.empty()) {
    std::println(os, "*RES");
    for (auto const &res : d_net.m_resistances) {
      std::println(
          os,
          "{} {} {} {}",
          res.m_id,
          res.m_node1,
          res.m_node2,
          res.m_res);
    }
  }
  std::println(os, "*END");
  return os;
}

std::ostream &operator<<(std::ostream &os, Port const &port) {
  std::print(os, "{} {}", port.m_name, get_direction_type_sv(port.m_direction));
  for (auto const &conn_attr : port.m_conn_attrs) {
    os << conn_attr.get();
  }
  std::println(os, "");
  return os;
}

std::ostream &operator<<(std::ostream &os, SPEF const &spef) {
  if (!spef.m_version.empty()) {
    std::println(os, "*SPEF {}", spef.m_version);
  }
  if (!spef.m_design_name.empty()) {
    std::println(os, "*DESIGN {}", spef.m_design_name);
  }
  if (!spef.m_date.empty()) {
    std::println(os, "*DATE {}", spef.m_date);
  }
  if (!spef.m_vendor.empty()) {
    std::println(os, "*VENDOR {}", spef.m_vendor);
  }
  if (!spef.m_program_name.empty()) {
    std::println(os, "*PROGRAM {}", spef.m_program_name);
  }
  if (!spef.m_program_version.empty()) {
    std::println(os, "*VERSION {}", spef.m_program_version);
  }
  if (!spef.m_design_flow.empty()) {
    std::println(os, "*DESIGN_FLOW {}", spef.m_design_flow);
  }
  if (spef.m_hierarchy_div_def != '\0') {
    std::println(os, "*DIVIDER {}", spef.m_hierarchy_div_def);
  }
  if (spef.m_pin_delim_def != '\0') {
    std::println(os, "*DELIMITER {}", spef.m_pin_delim_def);
  }
  if (spef.m_prefix_bus_delim != '\0') {
    if (spef.m_suffix_bus_delim != '\0') {
      std::println(
          os,
          "*BUS_DELIMITER {} {}",
          spef.m_prefix_bus_delim,
          spef.m_suffix_bus_delim);
    } else {
      std::println(os, "*BUS_DELIMITER {}", spef.m_prefix_bus_delim);
    }
  }
  if (spef.m_time_scale) {
    std::println(
        os,
        "*T_UNIT {} {}",
        spef.m_time_scale.value,
        spef.m_time_scale.unit);
  }
  if (spef.m_cap_scale) {
    std::println(
        os,
        "*C_UNIT {} {}",
        spef.m_cap_scale.value,
        spef.m_cap_scale.unit);
  }
  if (spef.m_res_scale) {
    std::println(
        os,
        "*R_UNIT {} {}",
        spef.m_res_scale.value,
        spef.m_res_scale.unit);
  }
  if (spef.m_induct_scale) {
    std::println(
        os,
        "*L_UNIT {} {}",
        spef.m_induct_scale.value,
        spef.m_induct_scale.unit);
  }

  if (!spef.m_power_nets.empty()) {
    std::print(os, "*POWER_NETS");
    for (auto const &power_net : spef.m_power_nets) {
      std::print(os, " {}", power_net);
    }
    std::println(os, "");
  }

  if (!spef.m_ground_nets.empty()) {
    std::print(os, "*GROUND_NETS");
    for (auto const &ground_net : spef.m_ground_nets) {
      std::print(os, " {}", ground_net);
    }
    std::println(os, "");
  }

  if (!spef.m_ports.empty()) {
    std::println(os, "*PORTS");
    for (auto const &port : spef.m_ports) {
      os << port;
    }
  }

  if (!spef.m_physcial_ports.empty()) {
    std::println(os, "*PHYSICAL_PORTS");
    for (auto const &pport : spef.m_physcial_ports) {
      std::println(
          os,
          "{} {}",
          pport.m_name,
          get_direction_type_sv(pport.m_direction));
    }
  }

  if (!spef.m_name_map.empty()) {
    std::println(os, "\n*NAME_MAP");
    // TODO: The order depends on the unordered map
    for (auto const &[index, name] : spef.m_name_map) {
      std::println(os, "{} {}", index, name);
    }
  }

  // first we write the D_NETs and then the R_NETs
  if (!spef.m_d_nets.empty()) {
    for (DNet const &d_net : spef.m_d_nets) {
      os << d_net;
    }
  }

  os << '\n';
  return os;
}

#endif  // SPEF_WRITE_HPP
