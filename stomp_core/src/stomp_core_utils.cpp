/*
 * Software License Agreement (Apache License)
 *
 * Copyright (c) 2016, Southwest Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * stomp_core_utils.cpp
 *
 *  Created on: Mar 9, 2016
 *      Author: Jorge Nicho
 */

#include <cmath>
#include <iostream>
#include <Eigen/Dense>
#include "stomp_core/stomp_core_utils.h"

namespace stomp_core
{


bool generateFiniteDifferenceMatrix(int num_time_steps,
                                             DerivativeOrders::DerivativeOrder order,
                                             double dt, Eigen::MatrixXd& diff_matrix)
{

  diff_matrix = Eigen::MatrixXd::Zero(num_time_steps, num_time_steps);
  double multiplier = 1.0/pow(dt,(int)order);
  for (int i=0; i<num_time_steps; ++i)
  {
    for (int j=-FINITE_DIFF_RULE_LENGTH/2; j<=FINITE_DIFF_RULE_LENGTH/2; ++j)
    {
      int index = i+j;
      if (index < 0)
      {
        index = 0;
        continue;
      }
      if (index >= num_time_steps)
      {
        index = num_time_steps-1;
        continue;
      }

      diff_matrix(i,index) = multiplier * FINITE_DIFF_COEFFS[order][j+FINITE_DIFF_RULE_LENGTH/2];
    }
  }
}

void differentiate(const Eigen::VectorXd& parameters, DerivativeOrders::DerivativeOrder order,
                          double dt, Eigen::VectorXd& derivatives )
{

  unsigned int padding = FINITE_DIFF_RULE_LENGTH/2;
  unsigned int padded_size = parameters.size()+FINITE_DIFF_RULE_LENGTH-1;
  Eigen::MatrixXd diff_matrix;
  Eigen::VectorXd padded_parameters = Eigen::VectorXd::Zero(padded_size);
  derivatives = Eigen::VectorXd::Zero(parameters.size());

  // initializing padded parameters
  padded_parameters.segment(FINITE_DIFF_RULE_LENGTH/2,parameters.size()) = parameters;
  padded_parameters.head(FINITE_DIFF_RULE_LENGTH/2).setConstant(parameters.head(1)(0));
  padded_parameters.tail(FINITE_DIFF_RULE_LENGTH/2).setConstant(parameters.tail(1)(0));

  // computing derivatives
  generateFiniteDifferenceMatrix(padded_size,order,dt,diff_matrix);
  derivatives = (diff_matrix*padded_parameters).col(0).segment(FINITE_DIFF_RULE_LENGTH/2,parameters.size());
  derivatives.head(1).setConstant(0.0);
  derivatives.tail(1).setConstant(0.0);
}

void toVector(const Eigen::MatrixXd& m,std::vector<Eigen::VectorXd>& v)
{
  v.resize(m.rows(),Eigen::VectorXd::Zero(m.cols()));
  for(auto d = 0u; d < m.rows();d++)
  {
    v[d] = m.row(d);
  }
}

std::string toString(const std::vector<Eigen::VectorXd>& data)
{
  Eigen::IOFormat clean_format(4, 0, ", ", "\n", "[", "]");
  Eigen::MatrixXd m = Eigen::MatrixXd::Zero(data.size(),data.front().size());
  std::stringstream ss;
  for(auto d = 0u; d < data.size(); d++)
  {
    m.row(d) = data[d].transpose();
  }

  ss<<m.format(clean_format);
  return ss.str();
}

std::string toString(const Eigen::MatrixXd& data)
{
  Eigen::IOFormat clean_format(4, 0, ", ", "\n", "[", "]");
  std::stringstream ss;
  ss<<data.format(clean_format);
  return ss.str();
}

std::string toString(const Eigen::VectorXd& data)
{
  Eigen::IOFormat clean_format(4, 0, ", ", "\n", "[", "]");
  std::stringstream ss;
  ss<<data.transpose().format(clean_format);
  return ss.str();
}

}




