#pragma once

void Phys_ObjAddForce(int id, const float *worldPos, const float *impulse, const bool relative);
void Phys_ObjGetCenterOfMass(int id, float *outPosition);