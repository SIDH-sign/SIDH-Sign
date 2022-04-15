
set(HDR
        elliptic_curve.h
        isogeny.h
        pok.h
        prime_field.h
        quadratic_field.h
        sidh.h
        signature.h
        utilities.h
        fips202.h
        twisted_edwards.h
        )


set(SRC
        elliptic_curve.c
        isogeny.c
        pok.c
        prime_field.c
        quadratic_field.c
        sidh.c
        signature.c
        utilities.c
        fips202.c
        twisted_edwards.c
        )

# Create a library target for each prime defined in parent PRIMES variable

set(COMP_OPT)

if (OPT MATCHES ASM)
    MESSAGE(STATUS "Using ASM optimization")
    set(COMP_OPT ${COMP_OPT} -march=native -mavx2 -fPIC -DQUADRATIC_FIELD_ASM)
endif ()

foreach (P IN LISTS PRIMES)
    message(STATUS "Creating library target sidh-pok-p${P}")
    if (OPT MATCHES ASM)
        set(SRCP
                P${P}/P${P}.S
                P${P}/P${P}_mul_sqr.S)
    else ()
        set(SRCP P${P}/P${P}.c)
    endif ()
    add_library(${PROJECT_NAME}-p${P} STATIC ${SRC} ${SRCP})
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC -D_P${P}_ ${COMP_OPT})
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
    target_compile_options(${PROJECT_NAME}-p${P} PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_FLAGS}>")
endforeach ()