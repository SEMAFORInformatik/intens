// **********************************************************************
// UsedInProjects
// **********************************************************************
FUNC used_in_projects_show_func {
  IF(!VALID(used_in_projects.element.comp_id)) {
    RETURN;
  }

  [used_in_projects.list[#]] = GET(
    PATH=
    [ "projects/component/"
    , used_in_projects.element.comp_id
    , "?varcomp="
    , used_in_projects.element.object]
  );

  MAP(used_in_projects_form);
};

FUNC used_in_projects_find_func{
  STRING message;
  INTEGER size, i;
  INTEGER c, l, projCnt, projSize;
  Component db_comp2;

  compInProjects.nfound = 0;
  CLEAR(compInProjects.projects, compInProjects.variants);
  project_save_data_library.deleted_result_comps = 0;

  IF( !VALID(compInProjects.vcomp) ){
    // Component not valid
    RETURN;
  }

  message = "<h2>Modify " + compInProjects.type + "</h2>"
  + "<table cellpadding='5', cellspacing='0', border='1'>"
  + "<tr><th>Name</th><th>Rev</th><th>Created</th><th>Approval</th>"
  + "<th>Owner</th><th>Group</th></tr><tr>"
  + "<td>" + compInProjects.name + "</td>"
  + "<td>" + compInProjects.rev + "</td>"
  + "<td>" + STRING_DATETIME(compInProjects.created) + "</td>"
  + "<td>" + DATASET_TEXT(compInProjects.approval) + "</td>"
  + "<td>" + compInProjects.ownername + "</td>"
  + "<td>" + compInProjects.group + "</td>"
  + "</tr></table>";

  // all projects that use this component
  [ compInProjects.projects ] = GET(
    PATH=["projects/component/", compInProjects.comp_id,
    "?varcomp=",compInProjects.vcomp]
  );
  SIZE(compInProjects.projects[*], size);
  IF( size>0 ){
    compInProjects.nfound=size;
    message = message
    + "<h3>Used in Projects</h3>"
    + "<table cellpadding='5', cellspacing='0', border='1'>"
    + "<tr><th>Name</th><th>Created</th><th>Status</th><th>Owner</th><th>Group</th></tr>";
    SIZE(project_save_data.project[*], projSize);
    i=0;
    WHILE( i<size ){
      message = message + "<tr>"
      + "<td>"+compInProjects.projects[i].name+"</td>"
      + "<td nowrap>"+STRING_DATETIME(compInProjects.projects[i].created)+"</td>"
      + "<td>"+DATASET_TEXT(compInProjects.projects[i].status)+"</td>"
      + "<td>"+compInProjects.projects[i].ownername+"</td>"
      + "<td nowrap>"+compInProjects.projects[i].group+"</td></tr>";
      i=i+1;
    }
    message=message+"</table>";
  }

  // find all variants of loaded project
  // that use the modified component (id: compInProjects.comp_id)
  // as compInProjects.vcomp
  projCnt = 0;
  WHILE(VALID(compInProjects.projects[projCnt].id)) {
    SIZE(project_save_data.project[*], projSize);
    // ProjectSaveData dialog data
    ASSIGN_CORR(project_save_data.project[projSize], compInProjects.projects[projCnt]);
    SET ( COLOR, project_save_data.project[projSize], 7 );
    project_save_data.project[projSize].folder = "╣";
    db_comp2.id = compInProjects.projects[projCnt].id;

    //  all projects/variants that use this component
    [ compInProjects.variants ] = GET(
      PATH=["variants/project/", db_comp2.id
    , "/component/", compInProjects.comp_id
    , "?varcomp=", compInProjects.vcomp]
    );

    SIZE(compInProjects.variants[*], size);
    IF( size > 0 ){
      compInProjects.nfound+=size;
      message = message
      + "<h3>Used in Variants of Workbook " + compInProjects.projects[projCnt].name +"</h3>"
      + "<table cellpadding='5', cellspacing='0', border='1'>"
      + "<tr><th>Name</th><th>Created</th><th>Approval</th><th>Owner</th><th>Group</th></tr>";
      SIZE(project_save_data.project[*], projSize);
      i=0;
      WHILE( i<size ){
        message = message
        + "<tr><td>"+compInProjects.variants[i].name+"</td>"
        + "<td>"+STRING_DATETIME(compInProjects.variants[i].created)+"</td>"
        + "<td>"+DATASET_TEXT(compInProjects.variants[i].approval)+"</td>"
        + "<td>"+compInProjects.variants[i].ownername+"</td>"
        + "<td>"+compInProjects.variants[i].group+"</td></tr>";

        // ProjectSaveData dialog data
        ASSIGN_CORR(project_save_data.project[projSize+i], compInProjects.variants[i]);
        CLEAR(project_save_data.project[projSize+i].id);
        SET ( COLOR, project_save_data.project[projSize+i], 8 );
        IF (i+1 == size) {
          project_save_data.project[projSize+i].folder = "╚═══";
        } ELSE {
          project_save_data.project[projSize+i].folder = "╠═══";
        }
        i++;
      }
      message=message+"</table>";
    }
    projCnt += 1;
  }
  IF( compInProjects.nfound==0 ){
    RETURN;
  }
  IF (VALID(project_save_data_library.save_mode_machine)) {
    // project dialog save, no result data in compInProjects
    compInProjects.nfound=0;
    compInProjects.message = INVALID;
  } ELSE {
    compInProjects.message = message;
  }
};
