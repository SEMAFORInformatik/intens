pushd ..

output="CMakeLists.txt_SOURCES"

rm ${output} 2> /dev/null

# source files with _("
grep -rl "\W_(\"" etc/ | sort >> ${output}
grep -rl "\W_(\"" intens/ | sort >> ${output}
grep -rl "\W_(\"" scripts/ | sort >> ${output}

sed -i -e 's/^\(.*\)$/        "\1"/' ${output}

popd
