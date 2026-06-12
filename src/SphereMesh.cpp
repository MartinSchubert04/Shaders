#include "SphereMesh.h"

SphereMesh::SphereMesh(int resolution) : _resolution(resolution) {
  genIcoSphere();
  buildMesh();
}

void SphereMesh::genIcoSphere() {
  verts.clear();
  tris.clear();
  std::map<std::pair<int, int>, int> midpointCache;

  const float t = (1.0f + sqrtf(5.0f)) / 2.0f;

  int divisions = std::max(0, _resolution);

  const float a =
      sqrt(1 / (1 + 2.618033988749895)); // value is the golden ratio
  const float c = a * 1.618033988749;

  Vector3 base[12] = {{-a, c, 0}, {a, c, 0}, {-a, -c, 0}, {a, -c, 0},
                      {0, -a, c}, {0, a, c}, {0, -a, -c}, {0, a, -c},
                      {c, 0, -a}, {c, 0, a}, {-c, 0, -a}, {-c, 0, a}};

  for (auto &v : base)
    verts.push_back(v);

  tris = {0, 11, 5,  0, 5,  1, 0, 1, 7, 0, 7,  10, 0, 10, 11, 1, 5, 9, 5, 11,
          4, 11, 10, 2, 10, 7, 6, 7, 1, 8, 3,  9,  4, 3,  4,  2, 3, 2, 6, 3,
          6, 8,  3,  8, 9,  4, 9, 5, 2, 4, 11, 6,  2, 10, 8,  6, 7, 9, 8, 1};

  for (int level = 0; level < divisions; level++) {
    std::vector<int> newTris;
    newTris.reserve(tris.size() * 4);
    for (size_t i = 0; i < tris.size(); i += 3) {
      int v0 = tris[i], v1 = tris[i + 1], v2 = tris[i + 2];
      int m01 = getMidpoint(v0, v1, verts, midpointCache);
      int m12 = getMidpoint(v1, v2, verts, midpointCache);
      int m02 = getMidpoint(v0, v2, verts, midpointCache);
      newTris.insert(newTris.end(), {
                                        v0, m01, m02, v1, m12, m01, v2, m02,
                                        m12, m01, m12, m02 // el del medio
                                    });
    }
    tris = std::move(newTris);
  }
}

void SphereMesh::buildMesh() {
  _mesh.vertexCount = (int)verts.size();
  _mesh.triangleCount = (int)tris.size() / 3;

  // Calculate normals
  std::vector<Vector3> normals(verts.size(), Vector3{0, 0, 0});
  for (size_t i = 0; i < tris.size(); i += 3) {
    Vector3 a = verts[tris[i]], b = verts[tris[i + 1]], c = verts[tris[i + 2]];
    Vector3 n =
        Vector3CrossProduct(Vector3Subtract(b, a), Vector3Subtract(c, a));
    normals[tris[i]] = Vector3Add(normals[tris[i]], n);
    normals[tris[i + 1]] = Vector3Add(normals[tris[i + 1]], n);
    normals[tris[i + 2]] = Vector3Add(normals[tris[i + 2]], n);
  }
  for (auto &n : normals)
    n = Vector3Normalize(n);

  _mesh.normals = (float *)MemAlloc(_mesh.vertexCount * 3 * sizeof(float));
  _mesh.vertices = (float *)MemAlloc(_mesh.vertexCount * 3 * sizeof(float));
  _mesh.colors =
      (unsigned char *)MemAlloc(_mesh.vertexCount * 4 * sizeof(unsigned char));

  const Color baseColor = RAYWHITE;

  for (int i = 0; i < _mesh.vertexCount; i++) {
    _mesh.vertices[i * 3 + 0] = verts[i].x;
    _mesh.vertices[i * 3 + 1] = verts[i].y;
    _mesh.vertices[i * 3 + 2] = verts[i].z;
    _mesh.normals[i * 3 + 0] = normals[i].x;
    _mesh.normals[i * 3 + 1] = normals[i].y;
    _mesh.normals[i * 3 + 2] = normals[i].z;
    _mesh.colors[i * 4 + 0] = baseColor.r;
    _mesh.colors[i * 4 + 1] = baseColor.g;
    _mesh.colors[i * 4 + 2] = baseColor.b;
    _mesh.colors[i * 4 + 3] = baseColor.a;
  }

  _mesh.indices =
      (unsigned short *)MemAlloc(tris.size() * sizeof(unsigned short));
  for (int i = 0; i < (int)tris.size(); i++)
    _mesh.indices[i] = (unsigned short)tris[i];

  UploadMesh(&_mesh, false);
}

int SphereMesh::getMidpoint(int a, int b, std::vector<Vector3> &verts,
                            std::map<std::pair<int, int>, int> &cache) {
  // Orden canónico: la arista (a,b) es la misma que (b,a)
  std::pair<int, int> key = std::minmax(a, b);

  auto it = cache.find(key);
  if (it != cache.end())
    return it->second; // ya existe: reusar vértice

  Vector3 mid =
      Vector3Normalize(Vector3Scale(Vector3Add(verts[a], verts[b]), 0.5f));
  verts.push_back(mid);
  int idx = (int)verts.size() - 1;
  cache[key] = idx;
  return idx;
}