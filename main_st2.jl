using JLD2
using StatsBase
using LinearAlgebra
using FastGaussQuadrature
using Optim

include("./SurfaceGeometry/dt20L/src/Iterators.jl")
include("./mesh_functions.jl")
include("./physics_functions.jl")
include("./mathematics_functions.jl")

## making the mesh
# points, faces = expand_icosamesh(;R=1,depth=2)
# points = Array{Float64}(points)
# faces = Array{Int64}(faces)


@load "./meshes/points_critical_hyst_2_21.jld2"
@load "./meshes/faces_critical_hyst_2_21.jld2"

points = Array{Float64}(points')
faces = Array{Int64}(faces')

a,b,c = maximum(points[1,:]), maximum(points[2,:]), maximum(points[3,:])

edges = make_edges(faces)
connectivity = make_connectivity(edges)
normals = Normals(points, faces)
(normals, CDE) = make_normals_spline(points, connectivity, edges, normals)

## setting simulation parameters
H0 = [0., 0., 1.]
mu = 30.
lambda = 10.
Bm = 0.

## calculating the magnetic field on the surface
psi = PotentialSimple(points, faces, normals, mu, H0)
Ht_vec = HtField(points, faces, psi, normals) # a vector
Ht = sqrt.(sum(Ht_vec.^2,dims=1))'
Hn = NormalFieldCurrent(points, faces, normals, Ht_vec, mu, H0) # a scalar

dHn = make_deltaH_normal(points, faces, normals, mu, H0; gaussorder=3)
Ht_aa = make_H_tangential(points, faces, normals, CDE, mu, H0, dHn; gaussorder = 3)'
Hn_aa = make_H_normal(dHn,mu)'

## testing against theoretical values
using QuadGK
function demag_coefs(a, b, c)

    upper_limit = 2000
    Rq2(q) = (a^2+q) * (b^2+q) * (c^2+q)

    Nx = a*b*c/2 * quadgk(s -> 1/(a^2+s) / sqrt(Rq2(s)), 0, upper_limit)[1]
    Ny = a*b*c/2 * quadgk(s -> 1/(b^2+s) / sqrt(Rq2(s)), 0, upper_limit)[1]
    Nz = a*b*c/2 * quadgk(s -> 1/(c^2+s) / sqrt(Rq2(s)), 0, upper_limit)[1]

    return [Nx, Ny, Nz]
end

function field_theor(a, b, c, mu, H0)

    Ns = demag_coefs(a,b,c)

    Hx = H0[1] / (1 + Ns[1] * (mu-1))
    Hy = H0[2] / (1 + Ns[2] * (mu-1))
    Hz = H0[3] / (1 + Ns[3] * (mu-1))

    return [Hx, Hy, Hz]
end

Hteor = field_theor(a, b, c, mu, H0)
Hn_teor = zeros(size(points,2))
Ht_teor = zeros(size(points,2))
tangs = zeros(size(normals))
for ykey in 1:size(points,2)
    Hn_teor[ykey] = dot(Hteor,normals[:,ykey])
    Ht_teor[ykey] = norm(cross(Hteor,normals[:,ykey]))
end

## plot the results
using Plots
plot(Hn_teor,label = "teor")
plot!(Hn,label = "old")
plot!(Hn_aa,label = "new")

plot(Ht_teor,label = "teor")
plot!(Ht,label = "old")
plot!(Ht_aa,label = "new")
