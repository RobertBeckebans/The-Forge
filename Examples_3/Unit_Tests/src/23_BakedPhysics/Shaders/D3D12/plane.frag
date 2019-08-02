/*
 * Copyright (c) 2018-2019 Confetti Interactive Inc.
 * 
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

// Shader for ground plane in Unit Tests Animation

struct VSOutput {
	  float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main(VSOutput input) : SV_TARGET
{
  float tol = 0.0025f;
  float res = 0.05f;

  float4 backgroundColor = float4(0.49f, 0.64f, 0.68f, 1.0f); // blue
  float4 lineColor = float4(0.39f, 0.41f, 0.37f, 1.0f); // grey
  float4 originColor = float4(0.0f, 0.0f, 0.0f, 1.0f); //black

  if ((abs(input.TexCoord.x - 0.5f) <= tol) && (abs(input.TexCoord.y - 0.5f) <= tol))
  {
   return originColor;
  }
  else if ((fmod(input.TexCoord.x, res) >= (res - tol)) || (fmod(input.TexCoord.x, res) < tol) || (fmod(input.TexCoord.y, res) >= (res - tol)) || (fmod(input.TexCoord.y, res) < tol))
  {
    return lineColor;
  }
  else
  {
    return backgroundColor;
  }
}
