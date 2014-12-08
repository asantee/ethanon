//
//  Shader.fsh
//  iOSBase
//
//  Created by Andre Santee on 8/12/14.
//  Copyright (c) 2014 Asantee Games. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
